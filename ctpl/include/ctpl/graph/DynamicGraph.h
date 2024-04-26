#pragma once

#include "IDynamicGraph.h"
#include <vector>
#include <unordered_map>

namespace ctpl {
    template<typename vertex_t, typename edge_props_t>
    class DynamicGraph : public IDynamicGraph<vertex_t, edge_props_t> {
        template<typename builder_self_t>
        using Builder = GraphBuilder<vertex_t, edge_props_t, builder_self_t>;
        using Visitor = IGraph<vertex_t, edge_props_t>::Visitor;
    public:

        template<typename builder_self_t>
        DynamicGraph(const Builder<builder_self_t>& builder) { // NOLINT
            for (const Edge<vertex_t, edge_props_t>& e : builder.edges()) {
                addEdge(e.u, e.v, e);
            }
        }

        bool isEdgeBelongs(vertex_t u, vertex_t v) const override {
            if (auto it = graph_.find(u); it != graph_.end()) {
                return it->second.contains(v);
            }
            return false;
        }

        void visitAdjacentVertices(vertex_t vertex, const Visitor& visitor) const override {
            if (auto it = graph_.find(vertex); it != graph_.end()) {
                for (const auto& [v, props] : it->second) {
                    visitor(vertex, v, props);
                }
            }
        }

        void visitAllEdges(const Visitor& visitor) const override {
            for (const auto& [u, list] : graph_) {
                for (const auto& [v, props] : list) {
                    if constexpr (has_prop<Undirected, edge_props_t>) {
                        if (u < v) {
                            visitor(u, v, props);
                        }
                    }
                    else {
                        visitor(u, v, props);
                    }
                }
            }
        }

        void addEdge(vertex_t u, vertex_t v, edge_props_t props) override {
            graph_[u][v] = props;
            if constexpr (has_prop<Undirected, edge_props_t>) {
                graph_[v][u] = props;
            }
        }

        void removeEdge(vertex_t u, vertex_t v) override {
            graph_[u].erase(v);
            if constexpr (has_prop<Undirected, edge_props_t>) {
                graph_[v].erase(u);
            }
        }

    private:
        std::unordered_map<vertex_t, std::unordered_map<vertex_t, edge_props_t>> graph_{};
    };
}
