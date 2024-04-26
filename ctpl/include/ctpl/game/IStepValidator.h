#pragma once

namespace ctpl::game {
    template<typename vertex_t>
    class IStepValidator {
    public:
        virtual ~IStepValidator() = default;

        virtual bool validateStep(vertex_t u, vertex_t v) = 0;
    };
}
