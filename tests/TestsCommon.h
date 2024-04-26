#pragma once

#include <gtest/gtest.h>
#include <ctpl/graph/IGraph.h>
#include <ctpl/graph/Builder.h>
#include <random>
#include <format>

using namespace ctpl;

namespace test {
    inline std::mt19937& rng() {
        static std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
        return rng;
    }

    using vertex_t = int;
    using AdjMatrix = std::vector<std::vector<bool>>;
    template<typename props_t>
    using Builder = GraphBuilder<vertex_t, props_t>;

    template<typename props_t, std::invocable<vertex_t, vertex_t> Visitor>
    void iterateOverPossibleEdges(std::size_t vertex_count, Visitor&& visitor) {
        for (vertex_t i = 0; i < vertex_count; ++i) {
            if constexpr (has_prop<Directed, props_t>) {
                for (vertex_t j = 0; j < i; ++j) {
                    visitor(i, j);
                }
            }
            for (vertex_t j = i + 1; j < vertex_count; ++j) {
                visitor(i, j);
            }
        }
    }

    template<typename props_t>
    AdjMatrix generateRandomAdjacencyMatrix(std::size_t vertex_count) {
        AdjMatrix matrix(vertex_count, std::vector<bool>(vertex_count, false));

        auto add_edge_maybe = [&](vertex_t u, vertex_t v) {
            if (rng()() & 1) {
                matrix[u][v] = true;
                if constexpr (has_prop<Undirected, props_t>) {
                    matrix[v][u] = true;
                }
            }
        };

        iterateOverPossibleEdges<props_t>(vertex_count, add_edge_maybe);

        return matrix;
    }

    namespace detail {
        template<typename graph_t, typename props_t>
        void building() {
            static_assert(std::is_base_of_v<IGraph<vertex_t, props_t>, graph_t>, "graph_t must be inherited from IGraph");

            using edge = Edge<vertex_t, props_t>;

            auto edges = []() -> std::vector<edge> {
                std::vector<edge> edges;
                for (vertex_t i = 3; i < 10; i++) {
                    edges.push_back({1, i, {}});
                }
                return edges;
            }();

            auto builder = GraphBuilder<vertex_t, props_t>()
                    .withEdge(0, 1, {})
                    .withEdge(1, 2, {})
                    .withEdge(2, 0, {})
                    .withEdges(edges);

            graph_t graph(builder);

            auto ASSERT_EDGE_BELONGS = [&](vertex_t u, vertex_t v) {
                ASSERT_TRUE(graph.isEdgeBelongs(u, v));
                if constexpr (has_prop<Undirected, props_t>) {
                    ASSERT_TRUE(graph.isEdgeBelongs(v, u));
                }
            };

            auto ASSERT_EDGE_NOT_BELONGS = [&](vertex_t u, vertex_t v) {
                ASSERT_FALSE(graph.isEdgeBelongs(u, v));
                if constexpr (has_prop<Undirected, props_t>) {
                    ASSERT_FALSE(graph.isEdgeBelongs(v, u));
                }
            };

            ASSERT_EDGE_BELONGS(0, 1);
            ASSERT_EDGE_BELONGS(1, 2);
            ASSERT_EDGE_BELONGS(2, 0);
            for (vertex_t i = 3; i < 10; i++) {
                ASSERT_EDGE_BELONGS(1, i);
            }
            for (vertex_t i = 0; i < 100; i++) {
                ASSERT_EDGE_NOT_BELONGS(1984, i);
            }
        }

        template<typename graph_t, typename props_t>
        void visitAllEdgesRandom() {
            static_assert(std::is_base_of_v<IGraph<vertex_t, props_t>, graph_t>, "graph_t must be inherited from IGraph");
            static constexpr std::size_t VERTEX_COUNT = 20;

            int times = 10;
            while (times--) {
                auto matrix = generateRandomAdjacencyMatrix<props_t>(VERTEX_COUNT);
                GraphBuilder<vertex_t, props_t> builder;
                iterateOverPossibleEdges<props_t>(VERTEX_COUNT, [&](vertex_t u, vertex_t v) {
                    if (matrix[u][v]) {
                        builder.withEdge(u, v, {});
                    }
                });

                graph_t graph(builder);
                std::vector<std::vector<std::size_t>> times_visited(VERTEX_COUNT, std::vector<std::size_t>(VERTEX_COUNT, 0));
                graph.visitAllEdges([&](vertex_t u, vertex_t v, auto props) {
                    times_visited[u][v]++;
                });

                iterateOverPossibleEdges<props_t>(VERTEX_COUNT, [&](vertex_t u, vertex_t v) {
                    if (matrix[u][v]) {
                        if constexpr (has_prop<Undirected, props_t>) {
                            ASSERT_EQ(times_visited[u][v] + times_visited[v][u], 1);
                            ASSERT_TRUE((times_visited[u][v] == 1 && times_visited[v][u] == 0) ||
                                        (times_visited[u][v] == 0 && times_visited[v][u] == 1));
                        }
                        else {
                            ASSERT_EQ(times_visited[u][v], 1);
                        }
                    }
                    else {
                        ASSERT_EQ(times_visited[u][v], 0);
                        if constexpr (has_prop<Undirected, props_t>) {
                            ASSERT_EQ(times_visited[v][u], 0);
                        }
                    }
                });
            }
        }

        template<typename graph_t, typename props_t>
        void visitAdjacentVerticesRandom() {
            static_assert(std::is_base_of_v<IGraph<vertex_t, props_t>, graph_t>, "graph_t must be inherited from IGraph");

            using edge = Edge<test::vertex_t, Undirected>;
            static constexpr std::size_t VERTEX_COUNT = 20;

            int times = 10;
            while (times--) {
                auto matrix = generateRandomAdjacencyMatrix<props_t>(VERTEX_COUNT);
                GraphBuilder<vertex_t, props_t> builder;
                iterateOverPossibleEdges<props_t>(VERTEX_COUNT, [&](vertex_t u, vertex_t v) {
                    if (matrix[u][v]) {
                        builder.withEdge(u, v, {});
                    }
                });

                graph_t graph = builder;

                for (vertex_t i = 0; i < VERTEX_COUNT; i++) {
                    std::vector<std::size_t> times_visited(VERTEX_COUNT);
                    graph.visitAdjacentVertices(i, [&](vertex_t u, vertex_t v, auto&&) {
                        if (u != i) {
                            throw std::logic_error(std::format("u-vertex must be equal to vertex from arguments, expected {}, got {}", i, u));
                        }
                        times_visited[v]++;
                    });
                    for (vertex_t j = 0; j < VERTEX_COUNT; j++) {
                        if (matrix[i][j]) {
                            ASSERT_EQ(times_visited[j], 1);
                        }
                        else {
                            ASSERT_EQ(times_visited[j], 0);
                        }
                    }
                }
            }
        }
    }
}
