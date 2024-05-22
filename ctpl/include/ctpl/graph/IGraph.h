#pragma once

#include "Edge.h"
#include <cstdint>
#include <functional>
#include <optional>

namespace ctpl {
    /**
     * @brief Base interface for graphs used in CTPL
     */
    template<typename vertex_t, typename edge_props_t, typename impl_t> //TODO concepts
    class IGraph {
    public:
        using vertex = vertex_t;
        using edge_props = edge_props_t;
        using Visitor = std::function<void(vertex_t, vertex_t, edge_props_t)>;

        void visitAllEdges(const Visitor &visitor) const {
            static_cast<const impl_t*>(this)->visitAllEdges(visitor);
        }

        void visitAdjacentVertices(vertex_t vertex, const Visitor &visitor) const {
            static_cast<const impl_t*>(this)->visitAdjacentVertices(vertex, visitor);
        }

        bool isEdgeBelongs(vertex_t u, vertex_t v) const {
            return static_cast<const impl_t*>(this)->isEdgeBelongs(u, v);
        }

        std::optional<edge_props_t> getEdgeProps(vertex_t u, vertex_t v) const {
            return static_Cast<const impl_t*>(this)->getEdgeProps(u, v);
        }
    };
}
