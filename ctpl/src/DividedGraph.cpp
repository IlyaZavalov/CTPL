#include <ctpl/game/outerplanar/DividedGraph.h>
#include <ctpl/util/assert.h>
#include <exception>
#include <format>
#include <unordered_set>

namespace ctpl::game {
    DividedOuterplanarGraph::BuildException::BuildException(const std::string &reason) :
            GraphBuildException(std::format(
                    "expect graph is outerplanar, graph sides starts in the source vertex, ends in the target vertex and do not intersects, actually: {}",
                    reason)) {
    }

    DividedOuterplanarGraph::DividedOuterplanarGraph(const DividedOuterplanarBuilder &builder) : super(builder),
                                                                                                 side_a_(builder.sideA()),
                                                                                                 side_b_(builder.sideB()) {
        auto &side_a = builder.sideA();
        auto &side_b = builder.sideB();

        if (side_a.size() < 2) {
            throw BuildException("side A vertices count is less than 2");
        }
        if (side_b.size() < 2) {
            throw BuildException("side B vertices count is less than 2");
        }

        if (side_a.front() != side_b.front()) {
            throw BuildException(
                    std::format("front vertices are not the same ({} != {})", side_a.front(), side_b.front()));
        }
        if (side_a.back() != side_b.back()) {
            throw BuildException(
                    std::format("back vertices are not the same ({} != {})", side_a.back(), side_b.back()));
        }

        vertex source_vertex = side_a.front();
        vertex target_vertex = side_b.back();

        auto build_vertex_set =
                [](const std::vector<vertex> &side, std::string_view side_desc) {
                    std::unordered_set<vertex> res;
                    for (auto u : side) {
                        if (res.contains(u)) {
                            throw BuildException(
                                    std::format("{} vertices are not unique ({} vertex has been listed several times)",
                                                side_desc, u));
                        }
                        res.insert(u);
                    }
                    return res;
                };

        auto side_a_indices = build_vertex_set(side_a, "side A");
        auto side_b_indices = build_vertex_set(side_b, "side B");

        auto check_vertex = [&](vertex u) {
            if (!side_a_indices.contains(u) && !side_b_indices.contains(u)) {
                throw BuildException(std::format("{} vertex is listed in edges but not part of side A or side B", u));
            }
        };

        for (const auto& e : builder.edges()) {
            check_vertex(e.u);
            check_vertex(e.v);
        }

        {
            auto vertex_by_index = [&](std::size_t index) {
                return index < side_a.size() ? side_a[index] : side_b[index - side_a.size() + 1];
            };
            std::size_t vertex_count = side_a.size() + side_b.size() - 2;
            for (std::size_t i = 0; i < vertex_count; i++) {
                auto u = vertex_by_index(i);
                super::visitAdjacentVertices(u, [&](auto&&, vertex v, auto&&) {
                    if (u >= v) {
                        return;
                    }
                    super::visitAdjacentVertices(v, [&](auto&&, vertex w, auto&&) {
                        if (v >= w) {
                            return;
                        }
                        if (!super::isEdgeBelongs(w, u)) {
                            return;
                        }

                        super::visitAdjacentVertices(w, [&](auto&&, vertex x, auto&&) {
                            if (w >= x) {
                                return;
                            }
                            if (!super::isEdgeBelongs(x, u) || !super::isEdgeBelongs(x, v) || !super::isEdgeBelongs(x, w)) {
                                return;
                            }

                            throw BuildException("graph is not outerplanar");
                        });
                    });
                });
            }

            for (std::size_t i = 0; i < vertex_count; i++) {
                auto u = vertex_by_index(i);
                for (std::size_t j = 0; j < i; j++) {
                    auto v = vertex_by_index(j);
                    std::unordered_set<vertex> s;
                    super::visitAdjacentVertices(u, [&](auto&&, vertex v, auto&&) {
                        s.insert(v);
                    });
                    super::visitAdjacentVertices(v, [&](auto&&, vertex v, auto&&) {
                        s.erase(v);
                    });
                    if (s.size() >= 3) {
                        throw BuildException("graph is not outerplanar");
                    }
                }
            }
        }
    }
}
