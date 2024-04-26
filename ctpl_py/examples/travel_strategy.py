import ctpl_py as ctpl
import random

class MyRound(ctpl.Round):
  def __init__(self, s, t, graph, traveller, adversary, ban_validator, step_validator):
      super().__init__(s, t, traveller, adversary, ban_validator, step_validator)
      self.graph = graph
      self.src_v = s
      self.tar_v = t
      self._traveller = traveller
      self._adversary = adversary
      self._ban_validator = ban_validator
      self._step_validator = step_validator


class MyTraveller(ctpl.ITraveller):
    def __init__(self, graph, src_v, tar_v):
        super().__init__()
        self._graph = graph
        self._cur_src = src_v
        self._cur_tar = tar_v
        self._dist = 0
        self._path = self._round.graph.shortestPath(src_v, tar_v)
        self._budget = 1
        self._side_a = graph.sideA()
        self._side_b = graph.sideB()
        self._side = None
        self._rolling_back = False
        self._path_index = 0

    def resourcing(self, v, z):
        self._cur_src = v
        self._path = self.graph.shortestPath(v, self._cur_tar)
        self._side = None
        a_index = self._side_a.index(v)
        b_index = self._side_b.index(z)
        if (v in self._side_a):
            a_index = a_index + 1
        else:
            b_index = b_index + 1
        self._side_a = self._side_a[a_index:]
        self._side_b = self._side_b[b_index:]
        self._budget = 1

    def isTC(self, u, v):
        if not self._graph.isEdgeBelongs(u, v):
            return False
        if (u == self._cur_tar or u == self._cur_src):
            return False
        if (v == self._cur_tar or v == self._cur_src):
            return False
        return ((u in self._side_a) != (v in self._side_a))

    def makeStep(self, u):
        if u == self._cur_tar:
            return

        if self._dist >= self._budget:
            self._rolling_back = True

        if self._rolling_back:
            if (self._path_index > 0):
                self._path_index = self._path_index - 1
                nxt = self._path[self._path_index]
                self._dist = self._dist + self._graph.getEdgeProps(nxt, u)
                return nxt
            else:
                self._rolling_back = False;
                self._path = self._graph.shortestPath(u, self._cur_tar)
                self._budget = 2 * self._budget

        best_tc = None
        best_dist = None
        
        def visitor(u, v, props):
            if not self.isTC(u, v):
                return

            if v in self._side_a:
                cur_dist = graph.shortestPath(v, self._cur_tar)
                if best_tc == None or best_dist > cur_dist:
                    best_tc = v
                    best_dist = cur_dist

        self._graph.visitAdjVertices(u, visitor)

        if best_tc is not None:
            self.resourcing(u, best_tc)
        
        nxt = self._path[self._path_index + 1]
        if self._graph.isEdgeBelongs(u, nxt):
            self._dist = self._dist + self.graph.getEdgeProps(u, nxt)['weight']
            self._path_index = self._path_index + 1
            return nxt
        
        if not isTC(u, nxt):
            self._rolling_back = True
            return self.makeStep(u)
        
        if (self._path_index == 0):
            self._path = self._graph.shortestPath(u, self._cur_tar)
        else:
            self._path = self._path[0:self._path_index - 1] + self._graph.shortestPath(u, self._cur_tar)
            
        return self.makeStep(u)            


    def notifyBanned(self, u, v):
        pass


class MyStepValidator(ctpl.IStepValidator):
    def __init__(self, graph):
        super().__init__()
        self._graph = graph

    def validateStep(self, u, v):
      return self._graph.isEdgeBelongs(u, v)


class KBanValidator(ctpl.IBanValidator):
  def __init__(self, k, traveller, graph):
      super().__init__(traveller)
      self._traveller = traveller
      self._graph = graph
      self._k = k

  def tryRemove(self, u, v):
    if self._k == 0:
        return False
    if not self._graph.isEdgeBelongs(u, v):
        return False
    self._k = self._k - 1
    self._graph.removeEdge(u, v)
    return True


class MyAdversary(ctpl.IAdversary):
  def __init__(self, ban_validator, graph, K):
    super().__init__(ban_validator)
    self._graph = graph
    self._ban_validator = ban_validator
    edges = []
    def visitor(u, v, props):
        edges.append((u, v))
    edges = random.sample(edges, k = K)
    b = GraphBuilder(ctpl.DIR.UNDIRECTED, ctpl.WEIGHT.WEIGHTED)
    for (u, v) in edges:
        b.withEdge(u, v)
    self._ban_graph = ctpl.Graph(b)

  def notifyTravellerStep(self, u, v):
    def visitor(u, v, props):
        self._ban_validator.requestBanEdge(u, v)
    self._ban_graph.visitAdjVertices(v, visitor)        


b = ctpl.GraphBuilder(ctpl.TYPES.DIVIDED_OUTERPLANAR)
b.withEdge(1, 2).withEdge(2, 3).withEdge(3, 4).withEdge(4, 5).withEdge(5, 9)
b.withEdge(1, 6).withEdge(6, 7).withEdge(7, 8).withEdge(8, 9)
b.withEdge(2, 4).withEdge(5, 6).withEdge(6, 8)
b.withSideA([1, 2, 3, 4, 5, 9])
b.withSideB([1, 6, 7, 8, 9])

g = ctpl.Graph(b)
tr = MyTraveller(g)
sv = MyStepValidator(g)
bv = MyBanValidator(tr, g)
adv = MyAdversary(bv, g)
r = MyRound(1, 4, g, tr, adv, bv, sv)
path = r.run()
