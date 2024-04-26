import ctpl_py as ctpl

class MyRound(ctpl.Round):
    def __init__(self, s, t, graph, traveller, adversary, ban_validator, step_validator):
        super().__init__(s, t, traveller, adversary, ban_validator, step_validator)
        self.graph = graph
        self._traveller = traveller
        self._adversary = adversary
        self._ban_validator = ban_validator
        self._step_validator = step_validator
class MyTraveller(ctpl.ITraveller):
    def __init__(self):
        super().__init__()

    def makeStep(self, u):
        print("now in", u)
        print("your step...")
        return int(input())

    def notifyBanned(self, u, v):
        print("notified banned", u, v)


class MyStepValidator(ctpl.IStepValidator):
    def __init__(self, graph):
        super().__init__()
        self._graph = graph

    def validateStep(self, u, v):
        print("validating step ", u, v)
        return self._graph.isEdgeBelongs(u, v)


class MyBanValidator(ctpl.IBanValidator):
    def __init__(self, traveller, graph):
        super().__init__(traveller)
        self._traveller = traveller
        self._graph = graph

    def tryRemove(self, u, v):
        print("trying remove", u, v)
        if (self._graph.isEdgeBelongs(u, v)):
            self._graph.removeEdge(u, v)
            return True
        else:
            return False


class MyAdversary(ctpl.IAdversary):
    def __init__(self, ban_validator, graph):
        super().__init__(ban_validator)
        self._graph = graph
        self._ban_validator = ban_validator

    def notifyTravellerStep(self, u, v):
        print("notify traveller step", u, v)
        self._ban_validator.tryRemove(3, 4)



b = ctpl.GraphBuilder(ctpl.DIR.UNDIRECTED, ctpl.WEIGHT.NOT_WEIGHTED)
b.withEdge(1, 2)
b.withEdge(2, 4)
b.withEdge(1, 3)
b.withEdge(3, 4)

g = ctpl.Graph(b)
tr = MyTraveller()
sv = MyStepValidator(g)
bv = MyBanValidator(tr, g)
adv = MyAdversary(bv, g)
r = MyRound(1, 4, g, tr, adv, bv, sv)
path = r.run()
print("path =", path)
