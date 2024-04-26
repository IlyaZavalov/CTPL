#pragma once

#include <ctpl/graph/IDynamicGraph.h>
#include <ctpl/game/ITraveller.h>

namespace ctpl::game {
    template<typename vertex_t>
    class IBanValidator {
    public:
        explicit IBanValidator(ITraveller<vertex_t>& traveller) : traveller_(traveller) {
        }

        bool requestBanEdge(vertex_t u, vertex_t v) {
            if (tryRemoveEdge(u, v)) {
                traveller_.notifyBanned(u, v);
                return true;
            }

            return false;
        }

        virtual ~IBanValidator() = default;

    protected:
        virtual bool tryRemoveEdge(vertex_t u, vertex_t v) = 0;

    private:
        ITraveller<vertex_t>& traveller_;
    };
}
