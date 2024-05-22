#pragma once

#include <optional>
#include <type_traits>

namespace ctpl {

    template<typename... Args>
    struct EdgeProps : Args ... {
    };

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

    template<typename prop_t, typename props_t>
    static constexpr bool has_prop = std::is_base_of_v<prop_t, props_t>;

    template<typename T>
    struct is_weighted {
        using underlying_type = std::nullopt_t;
        static constexpr bool value = false;
    };

    template<typename weight_t>
    struct is_weighted<Weighted<weight_t>> {
        using underlying_type = weight_t;
        static constexpr bool value = true;
    };

    template<typename edge_props_t>
    struct is_props_weighted;

    template<>
    struct is_props_weighted<EdgeProps<>> {
        static constexpr bool value = false;
        using underlying_type = std::nullopt_t;
    };

    template<typename T, typename... Args>
    struct is_props_weighted<EdgeProps<T, Args...>> {
        static constexpr bool value = is_weighted<T>::value || is_props_weighted<EdgeProps<Args...>>::value;
        using underlying_type = std::conditional_t<is_weighted<T>::value, typename is_weighted<T>::underlying_type, typename is_props_weighted<EdgeProps<Args...>>::underlying_type>;
    };

    template<typename edge_props_t>
    constexpr bool is_props_integral_weighted = is_props_weighted<edge_props_t>::value && std::is_integral_v<typename is_props_weighted<edge_props_t>::underlying_type>;

    template<typename edge_props_t>
    constexpr bool is_props_floating_weighted = is_props_weighted<edge_props_t>::value && std::is_floating_point_v<typename is_props_weighted<edge_props_t>::underlying_type>;

}
