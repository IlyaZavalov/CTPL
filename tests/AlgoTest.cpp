#include "TestsCommon.h"
#include <ctpl/graph/algo.h>
#include <ctpl/graph/Builder.h>
#include <ctpl/graph/DynamicGraph.h>

TEST(ShortestPath, Simple) {
    using weight_t = std::uint32_t;
    using props_t = EdgeProps<Undirected, Weighted<weight_t>>;
    using builder = GraphBuilder<test::vertex_t, props_t>;

    auto weightedEdge = [](weight_t w) {
        props_t res;
        res.weight = w;
        return res;
    };

    DynamicGraph<test::vertex_t, props_t> graph = builder()
            .withEdge(1, 2, weightedEdge(5))
            .withEdge(2, 4, weightedEdge(3))
            .withEdge(1, 3, weightedEdge(1))
            .withEdge(3, 4, weightedEdge(11));

    ASSERT_EQ(ctpl::dijkstra(graph, 1, 4), 8);
}
