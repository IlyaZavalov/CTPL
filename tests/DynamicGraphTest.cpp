#include "TestsCommon.h"

#include <ctpl/graph/DynamicGraph.h>
#include <ctpl/game/KBanValidator.h>
#include <ctpl/game/NonAdaptiveAdversary.h>

namespace test {
    template<typename props_t>
    void building() {
        detail::building<DynamicGraph<vertex_t, props_t>, props_t>();
    };

    template<typename props_t>
    void visitAllEdgesRandom() {
        detail::visitAllEdgesRandom<DynamicGraph<vertex_t, props_t>, props_t>();
    }

    template<typename props_t>
    void visitAdjacentVerticesRandom() {
        detail::visitAdjacentVerticesRandom<DynamicGraph<vertex_t, props_t>, props_t>();
    }
}

TEST(DynamicGraph, BuildingUndirected) {
    test::building<EdgeProps<Undirected>>();
}

TEST(DynamicGraph, BuildingDirected) {
    test::building<EdgeProps<Directed>>();
}

TEST(DynamicGraph, VisitAllEdges_Undirected_Random) {
    test::visitAllEdgesRandom<EdgeProps<Undirected>>();
}

TEST(DynamicGraph, VisitAllEdges_Directed_Random) {
    test::visitAllEdgesRandom<EdgeProps<Directed>>();
}

TEST(DynamicGraph, VisitAdjacentVertices_Undirected_Random) {
    test::visitAdjacentVerticesRandom<EdgeProps<Undirected>>();
}

TEST(DynamicGraph, VisitAdjacentVertices_Directed_Random) {
    test::visitAdjacentVerticesRandom<EdgeProps<Directed>>();
}
