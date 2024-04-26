#include <ctpl/graph/DynamicGraph.h>
#include <ctpl/game/Round.h>
#include <ctpl/game/outerplanar/DividedGraph.h>
#include <ctpl/util/assert.h>
#include <ctpl/graph/algo.h>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <optional>

using namespace pybind11::literals;

namespace ctpl::py {
    using vertex_t = game::default_vertex_t;
    using weight_t = game::weight_t;

    namespace detail {
        struct EdgeProps {
            std::optional<weight_t> weight = std::nullopt;

            [[nodiscard]]
            pybind11::dict toDict() const {
                if (weight) {
                    return pybind11::dict("weight"_a = *weight);
                }
                return {};
            }

            static EdgeProps fromDict(const pybind11::dict &d) {
                if (!d.contains("weight")) {
                    return {};
                }
                auto w = d["weight"];
                return {.weight = w.cast<weight_t>()};
            }
        };

        using EdgeVisitor = std::function<void(vertex_t, vertex_t, EdgeProps)>;

        class IGraph {
        public:
            virtual void visitAdjacentVertices(vertex_t u, const EdgeVisitor &visitor) = 0;

            virtual void visitAllEdges(const EdgeVisitor &visitor) = 0;

            virtual bool isEdgeBelongs(vertex_t u, vertex_t v) = 0;

            virtual void addEdge(vertex_t u, vertex_t v, EdgeProps e) = 0;

            virtual void removeEdge(vertex_t u, vertex_t v) = 0;

            virtual std::optional<EdgeProps> getEdgeProps(vertex_t u, vertex_t v) = 0;

            virtual const std::vector<vertex_t> &sideA() {
                throw std::runtime_error("side A getter is not implemented");
            }

            virtual const std::vector<vertex_t> &sideB() {
                throw std::runtime_error("side B getter is not implemented");
            }

            virtual std::vector<vertex_t> shortestPath(vertex_t s, vertex_t t) = 0;

            virtual std::optional<weight_t> shortestPathLength(vertex_t s, vertex_t t) = 0;

            virtual ~IGraph() = default;
        };

        template<typename graph_impl_t>
        class Graph : public IGraph {
            using vertex_t = graph_impl_t::vertex;
            using edge_props_t = graph_impl_t::edge_props;
        public:
            template<typename builder_t>
            explicit Graph(const builder_t &builder) : graph_(builder) {
            }

            void visitAdjacentVertices(vertex_t u, const EdgeVisitor &visitor) override {
                graph_.visitAdjacentVertices(u, [&](vertex_t u, vertex_t v, edge_props_t props) {
                    if constexpr (has_prop<Weighted<weight_t>, edge_props_t>) {
                        visitor(u, v, {.weight = props.weight});
                    } else {
                        visitor(u, v, {.weight = std::nullopt});
                    }
                });
            }

            void visitAllEdges(const EdgeVisitor &visitor) override {
                graph_.visitAllEdges([&](vertex_t u, vertex_t v, edge_props_t props) {
                    if constexpr (has_prop<Weighted<weight_t>, edge_props_t>) {
                        visitor(u, v, {.weight = props.weight});
                    } else {
                        visitor(u, v, {.weight = std::nullopt});
                    }
                });
            }

            bool isEdgeBelongs(vertex_t u, vertex_t v) override {
                return graph_.isEdgeBelongs(u, v);
            }

            void addEdge(vertex_t u, vertex_t v, EdgeProps e) override {
                if constexpr (has_prop<Weighted<weight_t>, edge_props_t>) {
                    edge_props_t p;
                    p.weight = e.weight.value_or(1);
                    graph_.addEdge(u, v, p);
                } else {
                    graph_.addEdge(u, v, {});
                }
            }

            void removeEdge(vertex_t u, vertex_t v) override {
                graph_.removeEdge(u, v);
            }

            std::optional<EdgeProps> getEdgeProps(vertex_t u, vertex_t v) override {
                if constexpr (has_prop<Weighted<weight_t>, edge_props_t>) {
                    std::optional<edge_props_t> props = graph_.getEdgeProps(u, v);
                    if (!props) {
                        return {};
                    }

                    return EdgeProps{
                            .weight = static_cast<Weighted<weight_t> &>(*props).weight
                    };

                } else {
                    return std::nullopt;
                }
            }

            const std::vector<vertex_t> &sideA() override {
                if constexpr (std::is_same_v<graph_impl_t, ::ctpl::game::DividedOuterplanarGraph>) {
                    return graph_.sideA();
                } else {
                    IGraph::sideA();
                }
                CTPL_UNREACHABLE();
            }

            const std::vector<vertex_t> &sideB() override {
                if constexpr (std::is_same_v<graph_impl_t, ::ctpl::game::DividedOuterplanarGraph>) {
                    return graph_.sideB();
                } else {
                    IGraph::sideB();
                }
                CTPL_UNREACHABLE();
            }

            std::vector<vertex_t> shortestPath(vertex_t s, vertex_t t) override {
                if constexpr (IsPropsWeighted<edge_props_t>::value) {
                    return ::ctpl::shortestPath(graph_, s, t);
                }
                else {
                    return {};
                }
            }

            std::optional<weight_t> shortestPathLength(vertex_t s, vertex_t t) override {
                if constexpr (IsPropsWeighted<edge_props_t>::value) {
                    if (auto w = ::ctpl::dijkstra(graph_, s, t)) {
                        return *w;
                    }
                    return std::nullopt;
                }
                else {
                    return std::nullopt;
                }
            }

        private:
            graph_impl_t graph_;
        };

        class IBuilder {
        public:
            virtual void addEdge(vertex_t u, vertex_t v, EdgeProps w) = 0;

            virtual std::unique_ptr<IGraph> build() = 0;

            virtual void withSideA(const std::vector<vertex_t> &side) {
                throw std::runtime_error("unimplemented");
            }

            virtual void withSideB(const std::vector<vertex_t> &side) {
                throw std::runtime_error("unimplemented");
            }

            virtual ~IBuilder() = default;
        };

        template<typename builder_impl_t>
        class Builder : public IBuilder {
            using vertex_t = builder_impl_t::vertex;
            using edge_props_t = builder_impl_t::edge_props;
        public:
            void addEdge(vertex_t u, vertex_t v, EdgeProps w) override {
                if constexpr (has_prop<Weighted<weight_t>, edge_props_t>) {
                    edge_props_t props;
                    props.weight = w.weight.value_or(1);
                    builder_.withEdge(u, v, props);
                } else {
                    builder_.withEdge(u, v, {});
                }
            }

            std::unique_ptr<IGraph> build() override {
                if constexpr (std::is_same_v<builder_impl_t, ::ctpl::game::DividedOuterplanarBuilder>) {
                    return std::make_unique<Graph<::ctpl::game::DividedOuterplanarGraph>>(builder_);
                } else {
                    return std::make_unique<Graph<DynamicGraph<vertex_t, edge_props_t>>>(builder_);
                }
            }

            void withSideA(const std::vector<vertex_t> &side) override {
                throw std::runtime_error("unimplemented");
            }

            void withSideB(const std::vector<vertex_t> &side) override {
                throw std::runtime_error("unimplemented");
            }

        private:
            builder_impl_t builder_;
        };

        template<typename... Args>
        inline std::unique_ptr<IBuilder> IBuilderCreateImpl(bool directed, bool weighted) {
            if constexpr (sizeof...(Args) == 0) {
                return (directed ? IBuilderCreateImpl<Args..., Directed> : IBuilderCreateImpl<Args..., Undirected>)(
                        directed,
                        weighted);
            } else if constexpr (sizeof...(Args) == 1) {
                return (weighted ? IBuilderCreateImpl<Args..., Weighted<weight_t>>
                                 : IBuilderCreateImpl<Args..., Unweighted>)(
                        directed, weighted);
            } else {
                static_assert(sizeof...(Args) == 2);
                return std::make_unique<Builder<::ctpl::GraphBuilder<vertex_t, ::ctpl::EdgeProps<Args...>>>>();
            }
        }

        inline std::unique_ptr<IBuilder> IBuilderCreate(bool directed, bool weighted) {
            return IBuilderCreateImpl<>(directed, weighted);
        }

    }

    enum class GraphDir {
        DIRECTED = 0,
        UNDIRECTED = 1
    };

    enum class GraphWeight {
        WEIGHTED = 0,
        UNWEIGHTED = 1
    };

    enum class GraphTypes {
        DIVIDED_OUTERPLANAR = 0
    };

    class GraphBuilder : public std::enable_shared_from_this<GraphBuilder> {
    public:
        explicit GraphBuilder(GraphTypes type) : impl_(
                std::make_unique<detail::Builder<::ctpl::game::DividedOuterplanarBuilder>>()) {
        }

        GraphBuilder(GraphDir dir, GraphWeight w) : impl_(
                detail::IBuilderCreate(dir == GraphDir::DIRECTED, w == GraphWeight::WEIGHTED)) {
        }

        std::shared_ptr<GraphBuilder> withEdgeW(vertex_t u, vertex_t v, weight_t w) {
            impl_->addEdge(u, v, {.weight = w});
            return shared_from_this();
        }

        std::shared_ptr<GraphBuilder> withEdge(vertex_t u, vertex_t v) {
            impl_->addEdge(u, v, {});
            return shared_from_this();
        }

        std::shared_ptr<GraphBuilder> withSideA(const std::vector<vertex_t>& v) {
            impl_->withSideA(v);
            return shared_from_this();
        }

        std::shared_ptr<GraphBuilder> withSideB(const std::vector<vertex_t>& v) {
            impl_->withSideB(v);
            return shared_from_this();
        }

        const std::unique_ptr<detail::IBuilder> &impl() const {
            return impl_;
        }

    private:
        std::unique_ptr<detail::IBuilder> impl_;
    };

    class Graph {
    public:
        explicit Graph(const std::shared_ptr<GraphBuilder> &builder) : impl_(builder->impl()->build()) {
        }

        void visitAdjacentVertices(vertex_t u, const std::function<void(vertex_t, vertex_t, pybind11::dict)> &visitor) {
            impl_->visitAdjacentVertices(u, [&](vertex_t u, vertex_t v, detail::EdgeProps props) {
                visitor(u, v, props.toDict());
            });
        }

        void visitAllEdges(const std::function<void(vertex_t, vertex_t, pybind11::dict)> &visitor) {
            impl_->visitAllEdges([&](vertex_t u, vertex_t v, detail::EdgeProps props) {
                visitor(u, v, props.toDict());
            });
        }

        bool isEdgeBelongs(vertex_t u, vertex_t v) {
            return impl_->isEdgeBelongs(u, v);
        }

        std::optional<detail::EdgeProps> getEdgeProps(vertex_t u, vertex_t v) {
            return impl_->getEdgeProps(u, v);
        }

        void addEdge(vertex_t u, vertex_t v, const pybind11::dict &edge_props) {
            impl_->addEdge(u, v, detail::EdgeProps::fromDict(edge_props));
        }

        void removeEdge(vertex_t u, vertex_t v) {
            impl_->removeEdge(u, v);
        }

        [[nodiscard]]
        const std::unique_ptr<detail::IGraph> &impl() const {
            return impl_;
        }

        [[nodiscard]]
        const std::vector<vertex_t> &sideA() const {
            return impl_->sideA();
        }

        [[nodiscard]]
        const std::vector<vertex_t> &sideB() const {
            return impl_->sideB();
        }

        auto shortestPath(vertex_t s, vertex_t t) const {
            return impl_->shortestPath(s, t);
        }

        auto shortestPathLength(vertex_t s, vertex_t t) const {
            return impl_->shortestPathLength(s, t);
        }

    private:
        std::unique_ptr<detail::IGraph> impl_;
    };

    class ITraveller : public ::ctpl::game::ITraveller<vertex_t> {
        using super = ::ctpl::game::ITraveller<vertex_t>;
    public:
        using super::super;

        vertex_t makeStep(vertex_t current_vertex) override = 0;
    };

    class IStepValidator : public ::ctpl::game::IStepValidator<vertex_t> {
        using super = ::ctpl::game::IStepValidator<vertex_t>;
    public:
        using super::super;

        bool validateStep(vertex_t u, vertex_t v) override = 0;
    };

    class IBanValidator : public ::ctpl::game::IBanValidator<vertex_t> {
        using super = ::ctpl::game::IBanValidator<vertex_t>;
    public:
        explicit IBanValidator(const std::shared_ptr<ITraveller> &traveller) : super(*traveller) {
        }

        bool tryRemoveEdge(vertex_t u, vertex_t v) override = 0;

    };

    class IAdversary : public ::ctpl::game::IAdversary<vertex_t> {
        using super = ::ctpl::game::IAdversary<vertex_t>;
    public:
        explicit IAdversary(const std::shared_ptr<IBanValidator> &validator) : super(*validator) {
        }

        void notifyTravellerStep(vertex_t u, vertex_t v) override = 0;
    };

    class PyTraveller : public ITraveller {
    public:
        using ITraveller::ITraveller;

        vertex_t makeStep(vertex_t current_vertex) override {
            PYBIND11_OVERLOAD_PURE(vertex_t, ITraveller, makeStep, current_vertex);
        }

        void notifyBanned(vertex_t u, vertex_t v) override {
            PYBIND11_OVERLOAD(void, ITraveller, notifyBanned, u, v);
        }

    };

    class PyBanValidator : public IBanValidator {
        using super = ::ctpl::game::IBanValidator<vertex_t>;
    public:
        using IBanValidator::IBanValidator;

        bool tryRemoveEdge(vertex_t u, vertex_t v) override {
            PYBIND11_OVERLOAD_PURE(bool, IBanValidator, tryRemoveEdge, u, v);
        }
    };

    class PyAdversary : public IAdversary {
    public:
        using IAdversary::IAdversary;

        void notifyTravellerStep(vertex_t u, vertex_t v) override {
            PYBIND11_OVERLOAD_PURE(void, IAdversary, notifyTravellerStep, u, v);
        }
    };

    class PyStepValidator : public IStepValidator {
    public:
        using IStepValidator::IStepValidator;

        bool validateStep(vertex_t u, vertex_t v) override {
            PYBIND11_OVERLOAD_PURE(bool, IStepValidator, validateStep, u, v);
        }
    };

    class PyRound : public ::ctpl::game::Round<vertex_t> {
        using super = ::ctpl::game::Round<vertex_t>;
    public:
        PyRound(vertex_t source_vertex, vertex_t target_vertex, const std::shared_ptr<PyTraveller> &traveller,
                const std::shared_ptr<PyAdversary> &adversary, const std::shared_ptr<PyBanValidator> &ban_validator,
                const std::shared_ptr<PyStepValidator> &step_validator) : super(source_vertex, target_vertex,
                                                                                *traveller,
                                                                                *adversary, *ban_validator,
                                                                                *step_validator) {
        }
    };
}

PYBIND11_MODULE(ctpl_py, m) {
    using namespace ::ctpl::py;

    pybind11::enum_<GraphDir>(m, "DIR")
            .value("DIRECTED", GraphDir::DIRECTED)
            .value("UNDIRECTED", GraphDir::UNDIRECTED)
            .export_values();

    pybind11::enum_<GraphWeight>(m, "WEIGHT")
            .value("WEIGHTED", GraphWeight::WEIGHTED)
            .value("NOT_WEIGHTED", GraphWeight::UNWEIGHTED)
            .export_values();

    pybind11::enum_<GraphTypes>(m, "TYPES")
            .value("DIVIDED_OUTERPLANAR", GraphTypes::DIVIDED_OUTERPLANAR);

    pybind11::class_<GraphBuilder, std::shared_ptr<GraphBuilder>>(m, "GraphBuilder")
            .def(pybind11::init<GraphDir, GraphWeight>())
            .def(pybind11::init<GraphTypes>())
            .def("withEdge", &GraphBuilder::withEdge)
            .def("withEdge", &GraphBuilder::withEdgeW)
            .def("withSideA", &GraphBuilder::withSideA)
            .def("withSideB", &GraphBuilder::withSideB);

    pybind11::class_<Graph, std::shared_ptr<Graph>>(m, "Graph")
            .def(pybind11::init<std::shared_ptr<GraphBuilder>>())
            .def("visitAdjVertices", &Graph::visitAdjacentVertices)
            .def("visitAllEdges", &Graph::visitAllEdges)
            .def("isEdgeBelongs", &Graph::isEdgeBelongs)
            .def("addEdge", &Graph::addEdge)
            .def("removeEdge", &Graph::removeEdge)
            .def("sideA", &Graph::sideA)
            .def("sideB", &Graph::sideB)
            .def("getEdgeProps", &Graph::getEdgeProps)
            .def("shortestPath", &Graph::shortestPath)
            .def("shortestPathLength", &Graph::shortestPathLength);

    pybind11::class_<ITraveller, PyTraveller, std::shared_ptr<ITraveller>>(m, "ITraveller")
            .def(pybind11::init<>())
            .def("makeStep", &ITraveller::makeStep)
            .def("notifyBanned", &ITraveller::notifyBanned);

    pybind11::class_<IStepValidator, PyStepValidator, std::shared_ptr<IStepValidator>>(m, "IStepValidator")
            .def(pybind11::init<>())
            .def("validateStep", &IStepValidator::validateStep);

    pybind11::class_<IBanValidator, PyBanValidator, std::shared_ptr<IBanValidator>>(m, "IBanValidator")
            .def(pybind11::init<std::shared_ptr<PyTraveller>>())
            .def("requestBanEdge", &IBanValidator::requestBanEdge)
            .def("tryRemove", &IBanValidator::tryRemoveEdge);

    pybind11::class_<IAdversary, PyAdversary, std::shared_ptr<IAdversary>>(m, "IAdversary")
            .def(pybind11::init<std::shared_ptr<IBanValidator>>())
            .def("notifyTravellerStep", &IAdversary::notifyTravellerStep);

    pybind11::class_<PyRound>(m, "Round")
            .def(pybind11::init<vertex_t, vertex_t, std::shared_ptr<PyTraveller>, std::shared_ptr<PyAdversary>, std::shared_ptr<PyBanValidator>, std::shared_ptr<PyStepValidator>>())
            .def("run", &::ctpl::game::Round<vertex_t>::run);
}
