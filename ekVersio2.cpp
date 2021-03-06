#include <iostream>
#include <queue>
#include <set>
#include <vector>

using namespace std;


struct Edge {
    int flow;
    int capacity;
    int prev;
    int next;
    int lwb;
    bool back;
};

bool operator<(const Edge& e1, const Edge& e2) {
    if (e1.prev == e2.prev) return e1.next < e2.next;
    return e1.prev < e2.prev;
}

bool operator==(const Edge& e1, const Edge& e2) {
    return (e1.prev == e2.prev && e1.next == e2.next
            && e1.flow == e2.flow && e1.capacity == e2.capacity
            && e1.lwb == e2.lwb && e1.back == e2.back);
}

bool operator!=(const Edge& e1, const Edge& e2) {
    return (!(e1 == e2));
}

struct Vertex {
    int airport;
    int time;
    int demand;
    set<Edge> adj;
};


void residual(const vector<Vertex>& G, vector<Vertex>& Gf) {
    /*  Vertices are mantained. Edges change:
        - Those edges with 0 free capacity do not appear
        - Those edges with positive free capacity:
            - Forward edge with value = (free capacity)
        - Those edges with positive flow:
            - Backward edge with value = (flow - lower bound)
     */

    for (int vf = 0; vf < Gf.size(); vf++)
        Gf[vf].adj.clear();


    for (Vertex v : G) {
        for (Edge e : v.adj) {
            if (e.capacity - e.flow > 0) {          // forward edges
                Edge ef = {0, e.capacity - e.flow, e.prev, e.next, 0, false};
                Gf[ef.prev].adj.insert(ef);
            }
            if (e.flow - e.lwb > 0) {               // backward edges
                Edge nef = {0, e.flow - e.lwb, e.next, e.prev, 0, true};
                Gf[nef.prev].adj.insert(nef);
            }
        }
    }
}

int bottleneck(const vector<Edge>& path) {
    int min = path[0].capacity;
    for (Edge e : path) {
        if (e.capacity < min) min = e.capacity;
    }
    return min;
}

int augment(const vector<Edge>& path, vector<Vertex>& G) {
    int b = bottleneck(path);
    for (Edge e : path) {

        // e is forward edge in G
        if (!e.back) {
            for (Edge augEdge : G[e.prev].adj) {
                if (augEdge.next == e.next) {
                    G[e.prev].adj.erase(augEdge);
                    augEdge.flow += b;
                    G[e.prev].adj.insert(augEdge);
                    break;
                }
            }
        }

        // e is backward edge in G
        else {
            for (Edge augEdge : G[e.next].adj) {
                if (augEdge.next == e.prev) {
                    G[e.next].adj.erase(augEdge);
                    augEdge.flow -= b;
                    G[e.next].adj.insert(augEdge);
                    break;
                }
            }
        }
    }
    return b;
}

vector<Edge> BFS(const vector<Vertex>& G, const int begin, const int end) {
    queue<Vertex> Q;
    vector<bool> visited(G.size(), false);
    vector<Edge> parent(G.size());

    Q.push(G[begin]);
    visited[begin] = true;

    Vertex w;
    while (not Q.empty()) {
        w = Q.front();
        Q.pop();
        if (w.airport == G[end].airport) {
            break;
        }

        for (Edge e : w.adj) {
            if (!visited[e.next]) {
                visited[e.next] = true;
                Q.push(G[e.next]);
                parent[e.next] = e;
            }
        }
    }

    Edge p = parent[end];
    vector<Edge> ret;
    if (p.next != end) return ret;
    while (p.prev != begin) {
        ret.push_back(p);
        p = parent[p.prev];
    }
    ret.push_back(p);
    reverse(ret.begin(), ret.end());
    return ret;
}

int edmondsKarp(vector<Vertex>& G, bool inverted) {
    vector<Vertex> Gf = G;
    residual(G, Gf);
    int flow = 0;

    int begin = Gf.size()-2;    // s
    int end = Gf.size()-1;      // t

    if (inverted) {
        begin = Gf.size()-1;    // t
        end = Gf.size()-2;      // s
    }

    vector<Edge> augPath = BFS(Gf, begin, end);
    while (!augPath.empty()) {
        flow += augment(augPath, G);
        residual(G, Gf);
        augPath = BFS(Gf, begin, end);
    }

    return flow;
}



void printSimplePath(vector<Vertex>& G, int v, int sink) {
    queue<Vertex> Q;
    vector<bool> visited(G.size(), false);
    vector<Edge> parent(G.size());

    Q.push(G[v]);
    visited[v] = true;

    Vertex w;
    while (not Q.empty()) {
        w = Q.front();
        Q.pop();
        if (w.airport == G[sink].airport) {
            break;
        }

        for (Edge e : w.adj) {
            if (!visited[e.next] and e.flow > 0) {
                visited[e.next] = true;
                Q.push(G[e.next]);
                parent[e.next] = e;
            }
        }
    }

    Edge p = parent[sink];
    vector<Edge> ret;
    if (p.next != sink) return;
    while (p.prev != v) {
        ret.push_back(p);
        p = parent[p.prev];
    }
    ret.push_back(p);
    reverse(ret.begin(), ret.end());

    bool first = true;
    for (int i = 0; i < ret.size(); i++) {
        if (ret[i].flow > 0 and ret[i].lwb == 1) {
            if (ret[i].flow == 1) {
                if (first) first = false;
                else cout << " ";
                cout << (ret[i].prev)/2 + 1;
            }
        }

        // Update original graph
        set<Edge> edges = G[ret[i].prev].adj;
        for (Edge e : edges) {
            if (e.next == ret[i].next) {
                G[ret[i].prev].adj.erase(e);
                e.flow -= 1;
                G[ret[i].prev].adj.insert(e);
            }
        }

    }
    cout << endl;

}

void printPaths(vector<Vertex>& G) {
    int sz = G.size();
    vector<int> starts;
    for (Edge startEdge : G[sz - 4].adj) {
        if (startEdge.flow > 0) {
            starts.push_back(startEdge.next);
        }
    }

    for (int v : starts) printSimplePath(G, v, sz - 3);
}



void updateK(vector<Vertex>& G, int k) {
    // update demands on: source, sink
    // update capacities on: edge ss - s, edge t - tt

    int sz = G.size();
    Edge fromSStoS = {0, -(G[sz - 4].demand), sz-2, sz-4, 0, false};
    Edge fromTtoTT = {0, G[sz - 3].demand, sz-3, sz-1, 0, false};

    Edge newSSS = fromSStoS;
    newSSS.capacity = k;
    Edge newTTT = fromTtoTT;
    newTTT.capacity = k;

    G[sz-4].demand = -k;
    G[sz-3].demand = k;

    G[sz-2].adj.erase(fromSStoS);
    G[sz-2].adj.insert(newSSS);

    G[sz-3].adj.erase(fromTtoTT);
    G[sz-3].adj.insert(newTTT);
}


// ================== MAIN =====================================================


int main() {
    int o, d, to, td;
    vector<Vertex> G;
    /*  landings[i] stores an array of indices j
        for all j, G[j] is a destination vertex with airport = i
    */
    vector<vector<int>> landings(2);
    while (cin >> o >> d >> to >> td) {

        // In airport - airport vertices, we apply lower bound <= demand reduction
        // source.demand += edge.lowerbound
        // dest.demand -= edge.lowerbound
        // correct flow/capacity in edge


        Vertex source = {o, to, 1};
        Vertex dest = {d, td, -1};
        G.push_back(source);
        G.push_back(dest);
        // edge from source to dest
        int sz = G.size();
        Edge e = {0, 0, sz - 2, sz - 1, 1, false};
        G[sz - 2].adj.insert(e);
        // store dest vertex in the "landings" array
        if (landings.size() < max(o, d) + 1) landings.resize(max(o, d) + 1);
        landings[d].push_back(sz - 1);
    }

    // Iterate over all source-vertexs (those with even index)
    for (int i = 0; i < G.size(); i += 2) {
        for (int j : landings[G[i].airport]) {
            if (G[i].time - G[j].time >= 15) {
                Edge e = {0, 1, j, i, 0, false};
                G[j].adj.insert(e);
            }
        }
    }

    int maxPilots = G.size() / 2;

    // Update capacities for version 2
    for (int v = 0; v < G.size(); v++) {
        set<Edge> vEdges = G[v].adj;
        G[v].adj.clear();
        for (Edge e : vEdges) {
            Edge newEdge = e;
            newEdge.capacity = maxPilots - e.lwb;
            G[v].adj.insert(newEdge);
        }
    }


    // Source and sink
    Vertex s{-1, -1, -maxPilots};  // negative demand means "want to send x units"
    Vertex t{-2, -2, maxPilots};
    G.push_back(s);
    G.push_back(t);
    int sz = G.size();
    for (int i = 0; i < sz - 2; i++) {
        if (i % 2 == 0) {
            Edge e = {1, maxPilots, sz - 2, i, 0, false};
            G[sz - 2].adj.insert(e);        // s --> G[i] (origin)
        } else {
            Edge e = {1, maxPilots, i, sz - 1, 0, false};
            G[i].adj.insert(e);             // G[i] --> t (destination)
        }
    }


    // Check maximum counter-flow: maxPilots - maxCounterFlow = minimum pilots required

    vector<Vertex> fresh = G;

    int maxCounterFlow = edmondsKarp(G, true);

    // Restore graph to the original form

    // Clean flow in edges s - u
    set<Edge> sEdges = fresh[G.size()-2].adj;
    fresh[G.size()-2].adj.clear();
    for (Edge e : sEdges) {
        e.flow = 0;
        fresh[G.size()-2].adj.insert(e);
    }

    // Clean flow in edges v - t
    for (int vt = 1; vt < fresh.size() - 2; vt += 2) {
        set<Edge> vertexEdges = fresh[vt].adj;
        for (Edge e : vertexEdges) {
            if (e.next == G.size()-1) {
                fresh[vt].adj.erase(e);
                e.flow = 0;
                fresh[vt].adj.insert(e);
                break;
            }
        }
    }

    // reduce "circulation with demands" to "maximum flow"
    // add super-source ss and super-sink tt
    // for each vertex with negative demand (send), add Edge(ss, v) with capacity -v.demand
    // for each vertex with positive demand (receive), add Edge(v, tt) with capacity v.demand

    Vertex ss = {-3, -3, 0};
    Vertex tt = {-4, -4, 0};
    fresh.push_back(ss);
    fresh.push_back(tt);
    sz = fresh.size();

    for (int i = 1; i < sz - 4; i += 2) {
        Edge e = {0, -(fresh[i].demand), sz-2, i, 0, false};
        fresh[sz-2].adj.insert(e);
    }
    for (int j = 0; j < sz - 4; j += 2) {
        Edge e = {0, fresh[j].demand, j, sz-1, 0, false};
        fresh[j].adj.insert(e);
    }


    Edge fromSStoS = {0, -(fresh[sz - 4].demand), sz-2, sz-4, 0, false};
    fresh[sz-2].adj.insert(fromSStoS);

    Edge fromTtoTT = {0, fresh[sz - 3].demand, sz-3, sz-1, 0, false};
    fresh[sz-3].adj.insert(fromTtoTT);

    updateK(fresh, maxPilots - maxCounterFlow);
    int optFlow = edmondsKarp(fresh, false) - maxPilots;

    cout << optFlow << endl;

    for (int v = 0; v < fresh.size() -4; v += 2) {
        set<Edge> vEdges = fresh[v].adj;
        for (Edge e : vEdges) {
            if (e.next == fresh.size()-1) {
                int flow = e.flow;
                for (Edge e2 : vEdges) {
                    if (e2.next == e2.prev + 1) {
                        Edge newEdge = e2;
                        fresh[v].adj.erase(e2);
                        newEdge.flow += flow;
                        fresh[v].adj.insert(newEdge);
                    }
                }
            }
        }
    }

    printPaths(fresh);

}
