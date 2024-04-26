#pragma once

#include <optional>
#include <type_traits>

namespace ctpl {

    template<typename... Args>
    struct EdgeProps : Args ... {
    };

    struct RuntimeEdgeProps {
    };

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

    struct Directed {
    };

    struct Undirected {
    };

    struct Unweighted {
    };

    template<typename T>
    struct IsWeighted {
        static constexpr bool value = false;
    };

    template<typename weight_t>
    struct IsWeighted<Weighted<weight_t>> {
        using underlying_type = weight_t;
        static constexpr bool value = true;
    };

    template<typename... Args>
    struct IsPropsWeighted;

    template<>
    struct IsPropsWeighted<> {
        static constexpr bool value = false;
        using underlying_type = std::nullopt_t;
    };

    template<typename T, typename... Args>
    struct IsPropsWeighted<EdgeProps<T, Args...>> {
        static constexpr bool value = IsWeighted<T>::value || IsPropsWeighted<EdgeProps<Args...>>::value;
        using underlying_type = std::conditional<IsWeighted<T>::value, typename IsWeighted<T>::underlying_type, typename IsPropsWeighted<EdgeProps<Args...>>::underlying_type>;
    };

}
