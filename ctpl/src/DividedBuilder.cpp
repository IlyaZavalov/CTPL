#include <ctpl/game/outerplanar/DividedBuilder.h>
#include <ctpl/util/assert.h>

namespace ctpl::game {
    template<typename Iterator>
    DividedOuterplanarBuilder &DividedOuterplanarBuilder::withSideA(Iterator begin, Iterator end) {
        CTPL_ASSERT(side_a_.empty(), "side A has been already set");
        std::copy(begin, end, std::back_inserter(side_a_));
        return *this;
    }

    template<typename Iterator>
    DividedOuterplanarBuilder &DividedOuterplanarBuilder::withSideB(Iterator begin, Iterator end) {
        CTPL_ASSERT(side_b_.empty(), "side B has been already set");
        std::copy(begin, end, std::back_inserter(side_b_));
        return *this;
    }

    DividedOuterplanarBuilder &DividedOuterplanarBuilder::withSideAVec(const std::vector<default_vertex_t> &v) {
        std::copy(v.begin(), v.end(), std::back_inserter(side_a_));
        return *this;
    }

    DividedOuterplanarBuilder &DividedOuterplanarBuilder::withSideBVec(const std::vector<default_vertex_t> &v) {
        std::copy(v.begin(), v.end(), std::back_inserter(side_b_));
        return *this;
    }
}
