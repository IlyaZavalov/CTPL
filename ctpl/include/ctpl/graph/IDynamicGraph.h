#pragma once

#include "IGraph.h"
#include "Builder.h"

namespace ctpl {
    template<typename vertex_t, typename edge_props_t>
    class IDynamicGraph : public IGraph<vertex_t, edge_props_t> {
    public:
        virtual void addEdge(vertex_t u, vertex_t v, edge_props_t props) = 0;

        virtual void removeEdge(vertex_t u, vertex_t v) = 0;
    };
}
