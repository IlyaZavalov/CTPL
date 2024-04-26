#pragma once

#include <ctpl/graph/Edge.h>
#include <ctpl/graph/IGraph.h>
#include <ctpl/graph/Builder.h>
#include <ctpl/util/assert.h>
#include <algorithm>
#include <vector>

namespace ctpl {
    template<typename vertex_t, typename edge_props_t>
    class IndexedGraph : IGraph<vertex_t, edge_props_t> {
        using AdjVertex = AdjacentVertex<vertex_t, edge_props_t>;
        using AdjList = std::vector<AdjVertex>;
        using AdjLists = std::vector<AdjList>;
        using Visitor = IGraph<vertex_t, edge_props_t>::Visitor;
        using Edge = Edge<vertex_t, edge_props_t>;
        using Builder = GraphBuilder<vertex_t, edge_props_t>;
    public:

        IndexedGraph(const Builder& builder) { // NOLINT
            CTPL_ASSERT(builder.vertexCount() != Builder::UNKNOWN_VERTEX_COUNT, "vertex count must be set");
            adj_lists_.resize(builder.vertex_count_);
            for (const Edge& e : builder.edges_) {
                CTPL_ASSERT(0 <= e.u && e.u < adj_lists_.size(), "vertex number is out of range");
                CTPL_ASSERT(0 <= e.v && e.v < adj_lists_.size(), "vertex number is out of range");
                adj_lists_[e.u].push_back(AdjVertex(e.v, e.props));
                if constexpr (has_prop<Undirected, edge_props_t>) {
                    adj_lists_[e.v].push_back(AdjVertex(e.u, e.props));
                }
            }

            for (AdjList& l : adj_lists_) {
                std::sort(l.begin(), l.end(), [](const AdjVertex& lhs, const AdjVertex& rhs) {
                    return lhs.v < rhs.v;
                });
            }
        }

        bool isEdgeBelongs(vertex_t u, vertex_t v) const override {
            CTPL_ASSERT(0 <= u && u < adj_lists_.size(), "vertex number is out of range");
            CTPL_ASSERT(0 <= v && v < adj_lists_.size(), "vertex number is out of range");
            auto it = std::lower_bound(adj_lists_[u].begin(), adj_lists_[u].end(), [](const AdjVertex& lhs, const AdjVertex& rhs) {
                return lhs.v < rhs.v;
            });
            return it != adj_lists_[u].end() && it->v == v;
        }

        std::optional<edge_props_t> getEdgeProps(vertex_t u, vertex_t v) const override {
            auto it = std::lower_bound(adj_lists_[u].begin(), adj_lists_[u].end(), [](const AdjVertex& lhs, const AdjVertex& rhs) {
                return lhs.v < rhs.v;
            });

            if (it == adj_lists_[u].end() || it->v != v) {
                return *it;
            }
        }

        void visitAdjacentVertices(vertex_t vertex, const Visitor& visitor) const override {
            CTPL_ASSERT(0 <= vertex && vertex < adj_lists_.size(), "vertex number is out of range");
            for (const AdjVertex& adj : adj_lists_[vertex]) {
                visitor(vertex, adj.v, adj);
            }
        }

        void visitAllEdges(const Visitor& visitor) const override {
            for (std::size_t u = 0; u < adj_lists_.size(); ++u) {
                for (const AdjVertex& adj : adj_lists_[u]) {
                    visitor(u, adj.v, adj);
                }
            }
        }

    private:
        AdjLists adj_lists_{};
    };

}
