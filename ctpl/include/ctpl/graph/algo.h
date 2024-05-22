#pragma once

#include <ctpl/graph/IGraph.h>
#include <concepts>
#include <limits>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>

namespace ctpl {

    template<
            typename vertex_t,
            typename edge_props_t,
            typename impl_t,
            std::invocable<vertex_t> Visitor,
            std::invocable<vertex_t> IsVisitedCallback,
            std::invocable<vertex_t, vertex_t> SetVisitedCallback
    >
    void dfsCustom(const IGraph<vertex_t, edge_props_t, impl_t> &graph, vertex_t initialVertex,
                   Visitor &&visitor, IsVisitedCallback &&is_visited, SetVisitedCallback &&set_visited) {
        std::stack<vertex_t> stack;
        stack.push(initialVertex);
        set_visited(initialVertex, initialVertex);

        while (!stack.empty()) {
            auto u = stack.top();
            stack.pop();
            visitor(u);
            graph.visitAdjacentVertices(u, [&](vertex_t, vertex_t v, edge_props_t) {
                if (is_visited(v)) {
                    return true;
                }
                stack.push(v);
                set_visited(u, v);
                return true;
            });
        }
    }

    template<
            typename vertex_t,
            typename edge_props_t,
            typename impl_t,
            std::invocable<vertex_t> Visitor,
            std::invocable<vertex_t> IsVisitedCallback,
            std::invocable<vertex_t, vertex_t> SetVisitedCallback
    >
    void bfsCustom(const IGraph<vertex_t, edge_props_t, impl_t> &graph, vertex_t initialVertex,
                   Visitor &&visitor, IsVisitedCallback &&isVisited, SetVisitedCallback &&setVisited) {
        std::queue<vertex_t> stack;
        stack.push(initialVertex);
        setVisited(initialVertex, initialVertex);

        while (!stack.empty()) {
            auto u = stack.top();
            stack.pop();
            visitor(u);
            graph.visitAdjacentVertices(u, [&](vertex_t, vertex_t v, edge_props_t) {
                if (isVisited(v)) {
                    return true;
                }
                stack.push(v);
                setVisited(u, v);
                return true;
            });
        }
    }

    template<
            typename vertex_t,
            typename edge_props_t,
            typename impl_t,
            std::invocable<vertex_t> IsVisitedCallback,
            std::invocable<vertex_t, vertex_t> SetVisitedCallback,
            std::invocable<vertex_t, typename is_props_weighted<edge_props_t>::underlying_type> SetDistCallback
    >
    void dijkstraCustom(const IGraph<vertex_t, edge_props_t, impl_t> &graph, vertex_t initial_vertex,
                        IsVisitedCallback &&is_visited, SetVisitedCallback &&set_visited, SetDistCallback &&set_dist) {
        using weight_t = is_props_weighted<edge_props_t>::underlying_type;
        static_assert(is_props_weighted<edge_props_t>::value, "dijkstra is applicable only to weighted graphs");
        struct PQVertex {
            vertex_t u;
            vertex_t v;
            weight_t dist;

            auto operator<=>(const PQVertex &other) const {
                return dist <=> other.dist;
            }
        };

        std::priority_queue<PQVertex, std::vector<PQVertex>, std::greater<PQVertex>> pq;
        pq.push({
                        .u = initial_vertex,
                        .v = initial_vertex,
                        .dist = 0
                });

        while (!pq.empty()) {
            auto [u, v, dist] = pq.top();
            pq.pop();
            if (is_visited(v)) {
                continue;
            }

            set_visited(u, v);
            set_dist(v, dist);
            graph.visitAdjacentVertices(v, [&](vertex_t u1, vertex_t v1, Weighted<weight_t> w) {
                if (is_visited(v1)) {
                    return;
                }
                pq.push({
                                .u = u1,
                                .v = v1,
                                .dist = dist + w.weight
                        });
            });
        }
    }

    template<typename vertex_t, typename edge_props_t, typename impl_t>
    auto dijkstra(const IGraph<vertex_t, edge_props_t, impl_t> &graph, vertex_t initial_vertex, vertex_t target_vertex) {
        std::unordered_set<vertex_t> used;
        using weight_t = is_props_weighted<edge_props_t>::underlying_type;
        std::optional<weight_t> res;
        dijkstraCustom(graph, initial_vertex,
                       [&](vertex_t u) {
                           return used.contains(u);
                       },
                       [&](vertex_t, vertex_t v) {
                           used.insert(v);
                       },
                       [&](vertex_t v, weight_t dist) {
                           if (v == target_vertex) {
                               res = dist;
                           }
                       });
        return res;
    }

    template<typename vertex_t, typename edge_props_t, typename impl_t>
    std::vector<vertex_t>
    shortestPath(const IGraph<vertex_t, edge_props_t, impl_t> &graph, vertex_t initial_vertex, vertex_t target_vertex) {
        std::unordered_map<vertex_t, vertex_t> parents;
        dijkstraCustom(graph, initial_vertex, [&](vertex_t u) { return parents.contains(u); },
                       [&](vertex_t u, vertex_t v) { parents[v] = u; }, [](vertex_t, vertex_t) {});
        std::vector<vertex_t> path = {target_vertex};
        if (!parents.contains(target_vertex)) {
            return {};
        }

        std::reverse(path.begin(), path.end());

        while (path.back() != initial_vertex) {
            path.push_back(parents[path.back()]);
        }

        return path;
    }

}
