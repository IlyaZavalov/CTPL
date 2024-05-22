#pragma once

#include "IGraph.h"
#include "Builder.h"

namespace ctpl {
    template<typename vertex_t, typename edge_props_t, typename impl_t>
    class IDynamicGraph : public IGraph<vertex_t, edge_props_t, impl_t> {
    public:
        void addEdge(vertex_t u, vertex_t v, edge_props_t props) {
            return static_cast<impl_t*>(this)->addEdge(u, v, props);
        }

        void removeEdge(vertex_t u, vertex_t v) {
            return static_cast<impl_t*>(this)->removeEdge(u, v);
        }
    };
}
