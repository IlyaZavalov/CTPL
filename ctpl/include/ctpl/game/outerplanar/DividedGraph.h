#pragma once

#include <ctpl/game/common.h>
#include <ctpl/game/outerplanar/common.h>
#include <ctpl/game/outerplanar/DividedBuilder.h>
#include <ctpl/graph/DynamicGraph.h>
#include <ctpl/graph/Builder.h>

namespace ctpl::game {
    class DividedOuterplanarGraph : public DynamicGraph<default_vertex_t, outerplanar_props_t> {
        using super = DynamicGraph<default_vertex_t, outerplanar_props_t>;
    public:
        class BuildException : public GraphBuildException {
        public:
            explicit BuildException(const std::string& reason);
        };

        DividedOuterplanarGraph(const DividedOuterplanarBuilder& builder); // NOLINT

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
