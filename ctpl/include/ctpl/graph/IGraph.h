#pragma once

#include "Edge.h"
#include <cstdint>
#include <functional>
#include <optional>

namespace ctpl {
    /**
     * @brief Base interface for graphs used in CTPL
     * @todo CTPL is based on templates, so static polymorphism would be more appropriate there
     */
    template<typename vertex_t, typename edge_props_t = EdgeProps<Undirected>> //TODO concepts
    class IGraph {
    public:
        using vertex = vertex_t;
        using edge_props = edge_props_t;
        using Visitor = std::function<void(vertex_t, vertex_t, edge_props_t)>;

        virtual ~IGraph() = default;

        virtual void visitAllEdges(const Visitor &visitor) const = 0;

        virtual void visitAdjacentVertices(vertex_t vertex, const Visitor &visitor) const = 0;

        virtual bool isEdgeBelongs(vertex_t u, vertex_t v) const = 0;

        virtual std::optional<edge_props_t> getEdgeProps(vertex_t u, vertex_t v) const = 0;
    };
}
