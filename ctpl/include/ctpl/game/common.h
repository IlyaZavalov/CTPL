#pragma once

#include <ctpl/graph/Edge.h>
#include <cstdint>
#include <stdexcept>

namespace ctpl::game {
    using default_vertex_t = std::uint32_t;
    using weight_t = std::int32_t;

    class GraphBuildException : public std::runtime_error {
        using super = std::runtime_error;
    public:
        using super::super;
    };
}
