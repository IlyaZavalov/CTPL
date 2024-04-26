#pragma once

#include <ctpl/game/ITraveller.h>
#include <ctpl/game/IAdversary.h>
#include <ctpl/game/IBanValidator.h>
#include <ctpl/game/IStepValidator.h>
#include <memory>

namespace ctpl::game {
    template<typename vertex_t>
    class Round {
    public:
        Round(vertex_t source_vertex, vertex_t target_vertex, ITraveller<vertex_t> &traveller,
              IAdversary<vertex_t> &adversary, IBanValidator<vertex_t> &ban_validator,
              IStepValidator<vertex_t> &step_validator)
                : source_vertex_(source_vertex), target_vertex_(target_vertex), traveller_(traveller),
                  adversary_(adversary), ban_validator_(ban_validator), step_validator_(step_validator) {
        }

        std::vector<vertex_t> run() {
            std::vector<vertex_t> traveller_path = {source_vertex_};
            while (traveller_path.back() != target_vertex_) {
                auto current_vertex = traveller_path.back();
                auto next_vertex = traveller_.makeStep(traveller_path.back());
                if (!step_validator_.validateStep(current_vertex, next_vertex)) {
                    continue;
                }
                traveller_path.push_back(next_vertex);
                adversary_.notifyTravellerStep(current_vertex, next_vertex);
            }

            return traveller_path;
        }


    private:
        vertex_t source_vertex_;
        vertex_t target_vertex_;
        ITraveller<vertex_t> &traveller_;
        IAdversary<vertex_t> &adversary_;
        IBanValidator<vertex_t> &ban_validator_;
        IStepValidator<vertex_t> &step_validator_;
    };
}
