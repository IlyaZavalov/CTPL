#pragma once

#include <type_traits>

namespace ctpl {

    template<typename... Args>
    struct EdgeProps : Args... {
    };

    struct RuntimeEdgeProps{};

    template<typename prop_t, typename props_t>
    static constexpr bool has_prop = std::is_base_of_v<prop_t, props_t>;

    template<typename vertex_t, typename edge_props_t = EdgeProps<>>
    struct Edge : edge_props_t {
        Edge(vertex_t u1, vertex_t v1, edge_props_t props) : edge_props_t(props), u(u1), v(v1) {
        }

        vertex_t u;
        vertex_t v;
    };

    template<typename vertex_t, typename edge_props_t = EdgeProps<>>
    struct AdjacentVertex : edge_props_t {
        AdjacentVertex(vertex_t vertex, edge_props_t props) : edge_props_t((props)), v(vertex) {
        }

        vertex_t v;
    };

    // props

    template<typename weight_t>
    struct Weighted {
        weight_t weight;
    };

    struct Directed {};

    struct Undirected {};

    struct Unweighted {};
}
