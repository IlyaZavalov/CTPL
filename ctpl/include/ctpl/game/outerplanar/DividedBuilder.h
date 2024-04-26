#pragma once

#include <ctpl/graph/DynamicGraph.h>
#include <ctpl/graph/Builder.h>

namespace ctpl::game {
    using default_vertex_t = std::uint32_t;
    using weight_t = std::int32_t;
    using outerplanar_props_t = EdgeProps<Undirected, Weighted<weight_t>>;

    class DividedOuterplanarBuilder : public GraphBuilder<default_vertex_t, outerplanar_props_t, DividedOuterplanarBuilder> {
        using self = DividedOuterplanarBuilder;
    public:
        DividedOuterplanarBuilder() = default;

        template<typename Iterator>
        DividedOuterplanarBuilder& withSideA(Iterator begin, Iterator end);

        template<typename Cont>
        DividedOuterplanarBuilder& withSideA(Cont&& cont) {
            return withSideA(cont.begin(), cont.end());
        }

        template<typename Iterator>
        DividedOuterplanarBuilder& withSideB(Iterator begin, Iterator end);

        template<typename Cont>
        DividedOuterplanarBuilder& withSideB(Cont&& cont) {
            return withSideB(cont.begin(), cont.end());
        }

        [[nodiscard]]
        const std::vector<default_vertex_t>& sideA() const noexcept {
            return side_a_;
        }

        [[nodiscard]]
        const std::vector<default_vertex_t>& sideB() const noexcept {
            return side_b_;
        }

    private:
        std::vector<default_vertex_t> side_a_;
        std::vector<default_vertex_t> side_b_;
    };
}
