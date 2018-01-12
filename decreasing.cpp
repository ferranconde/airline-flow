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
            - Backward edge with value = (flow)
     */

    for (int vf = 0; vf < Gf.size(); vf++)      // TODO: NO PASSAR A for range!!!!!!
        Gf[vf].adj.clear();


    for (Vertex v : G) {
        for (Edge e : v.adj) {
            if (e.capacity - e.flow > 0) {  // forward edges
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

    Q.push(G[begin]);    // push super-source vertex
    visited[begin] = true;

    Vertex w;
    while (not Q.empty()) {
        w = Q.front();
        Q.pop();
        if (w.airport == G[end].airport) {  // super-sink (tt)
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

    Edge p = parent[end];  // (tt)
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
        begin = Gf.size()-1;
        end = Gf.size()-2;
    }

    vector<Edge> augPath = BFS(Gf, begin, end);
    while (!augPath.empty()) {
        flow += augment(augPath, G);
        residual(G, Gf);
        augPath = BFS(Gf, begin, end);
    }

    return flow;
}

int countStarts(const vector<Vertex>& G) {
    int count = 0;
    int sz = G.size();
    for (Edge e : G[sz - 2].adj) {
        if (e.flow > 0 and e.next != sz-1) count++;
    }
    return count;
}

void printSimplePath(const vector<Vertex>& G, int v, int sink) {
    queue<Vertex> Q;
    vector<bool> visited(G.size(), false);
    vector<Edge> parent(G.size());

    Q.push(G[v]);                           // push start vertex
    visited[v] = true;

    Vertex w;
    while (not Q.empty()) {
        w = Q.front();
        Q.pop();
        if (w.airport == G[sink].airport) {  // sink
            break;
        }

        for (Edge e : w.adj) {
            if (!visited[e.next] and (e.flow > 0 or e.capacity == 0)) {
                visited[e.next] = true;
                Q.push(G[e.next]);
                parent[e.next] = e;
            }
        }
    }

    Edge p = parent[sink];  // (tt)
    vector<Edge> ret;
    if (p.next != sink) return;
    while (p.prev != v) {
        ret.push_back(p);
        p = parent[p.prev];
    }
    ret.push_back(p);
    reverse(ret.begin(), ret.end());

    for (int i = 0; i < ret.size(); i++) {
        if (ret[i].capacity == 0) cout << (ret[i].prev)/2 + 1 << " ";
    }
    cout << endl;

}

void printPaths(const vector<Vertex>& G) {
    int sz = G.size();
    vector<int> starts;
    for (Edge startEdge : G[sz - 2].adj) {
        if (startEdge.flow > 0) {
            starts.push_back(startEdge.next);
        }
    }

    for (int v : starts) printSimplePath(G, v, sz - 1);
}


bool existsPath(int v, int sink, const vector<Vertex>& G) {
    queue<Vertex> Q;
    vector<bool> visited(G.size(), false);
    vector<Edge> parent(G.size());

    Q.push(G[v]);                           // push start vertex
    visited[v] = true;

    Vertex w;
    while (not Q.empty()) {
        w = Q.front();
        Q.pop();
        if (w.airport == G[sink].airport) {  // sink
            break;
        }

        for (Edge e : w.adj) {
            if (!visited[e.next] and (e.flow > 0 or e.capacity == 0)) {
                visited[e.next] = true;
                Q.push(G[e.next]);
                parent[e.next] = e;
            }
        }
    }

    Edge p = parent[sink];  // (tt)
    vector<Edge> ret;

    return (p.next == sink);
}

int countPaths(const vector<Vertex>& G) {
    int sz = G.size();
    vector<int> starts;
    for (Edge startEdge : G[sz-2].adj) {
        if (startEdge.flow > 0) {
            starts.push_back(startEdge.next);
        }
    }

    int count = 0;
    for (int v : starts) {
        if (existsPath(v, sz - 1, G)) count++;
    }
    return count;
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


        Vertex source = {o, to, 0};
        Vertex dest = {d, td, 0};
        G.push_back(source);
        G.push_back(dest);
        // edge from source to dest
        int sz = G.size();
        Edge e = {1, 1, sz - 2, sz - 1, 1, false};      // remember: new capacity = capacity - lower bound
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

    // Source and sink
    Vertex s{-1, -1, -maxPilots};  // negative demand means "want to send x units"
    Vertex t{-2, -2, maxPilots};
    G.push_back(s);
    G.push_back(t);
    int sz = G.size();
    for (int i = 0; i < sz - 2; i++) {
        if (i % 2 == 0) {
            Edge e = {1, 1, sz - 2, i, 0, false};
            G[sz - 2].adj.insert(e);  // s --> G[i] (origin)
        } else {
            Edge e = {1, 1, i, sz - 1, 0, false};
            G[i].adj.insert(e);  // G[i] --> t (destination)
        }
    }

    // add extra edge between source and sink to derive excess of flow
    // Edge extra = {0, maxPilots, sz - 2, sz - 1, 0, false};
    // G[sz - 2].adj.insert(extra);


    int currentFlow = edmondsKarp(G, true);
    //int starts = countStarts(G);


}