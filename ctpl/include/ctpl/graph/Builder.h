#pragma once

#include "Edge.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace ctpl {
    struct this_ph{};

    template<typename vertex_t, typename edge_props_t, typename self_t = this_ph>
    class GraphBuilder {
        using edge = Edge<vertex_t, edge_props_t>;
        using self = std::conditional_t<std::is_same_v<self_t, this_ph>,
                                        GraphBuilder<vertex_t, edge_props_t, this_ph>,
                                        self_t>;
    public:
        using vertex = vertex_t;
        using edge_props = edge_props_t;
        static constexpr std::size_t UNKNOWN_VERTEX_COUNT = -1;

        GraphBuilder() = default;

        self& withVertexCount(std::size_t count) {
            vertex_count_ = count;
            return *static_cast<self*>(this);
        }

        self& withEdge(edge e) {
            edges_.push_back(e);
            return *static_cast<self*>(this);
        }

        self& withEdge(vertex_t u, vertex_t v, edge_props_t props) {
            edges_.push_back(edge(u, v, props));
            return *static_cast<self*>(this);
        }

        template<typename Iterator>
        self& withEdges(Iterator begin, Iterator end) {
            std::for_each(begin, end, [this](const edge& e) {
                edges_.push_back(e);
            });
            return *static_cast<self*>(this);
        }

        template<typename Container>
        self& withEdges(Container&& cont) {
            withEdges(cont.begin(), cont.end());
            return *static_cast<self*>(this);
        }

        const std::vector<edge>& edges() const noexcept {
            return edges_;
        }

        [[nodiscard]]
        std::size_t vertexCount() const noexcept {
            return vertex_count_;
        }

    private:
        std::size_t vertex_count_ = UNKNOWN_VERTEX_COUNT;
        std::vector<edge> edges_{};
    };
}