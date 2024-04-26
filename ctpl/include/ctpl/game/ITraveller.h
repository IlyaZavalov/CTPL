#pragma once

namespace ctpl::game {
    template<typename vertex_t>
    class ITraveller {
    public:
        virtual ~ITraveller() = default;

        virtual vertex_t makeStep(vertex_t current_vertex) = 0;

        virtual void notifyBanned(vertex_t u, vertex_t v) {}
    };
}
