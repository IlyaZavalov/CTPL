#pragma once

#include <ctpl/game/IBanValidator.h>
#include <memory>

namespace ctpl::game {
    template<typename vertex_t>
    class IAdversary {
    public:
        explicit IAdversary(IBanValidator<vertex_t>& v) : validator_(v) {
        }

        virtual ~IAdversary() = default;

        virtual void notifyTravellerStep(vertex_t u, vertex_t v) = 0;

    protected:
        bool tryBanEdge(vertex_t u, vertex_t v) {
            return validator_.requestBanEdge(u, v);
        }

    private:
        IBanValidator<vertex_t>& validator_;
    };
}
