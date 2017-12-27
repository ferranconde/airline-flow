#include <iostream>
#include <vector>
#include <set>
using namespace std;


struct Edge {
    int flow;
    int capacity;
    int prev;
    int next;
};

bool operator<(const Edge& e1, const Edge& e2) {
    if (e1.prev == e2.prev) return e1.next < e2.next;
    return e1.prev < e2.prev;
}

struct Vertex {
    int airport;
    int time;
    set<Edge> adj;
};

/*
    TODO:
        - Create residual network Gf
        - BFS algorithm
        - Augment algorithm
        - Edmonds-Karp algorithm
        - Check how to manage lower bounds properly
*/

void residual(const vector<Vertex>& G, vector<Vertex>& Gf) {
    /*  Vertices are mantained. Edges change:
        - Those edges with 0 free capacity do not appear
        - Those edges with positive free capacity:
            - Forward edge with value = (free capacity)
        - Those edges with positive flow:
            - Backward edge with value = (flow)

        For convention, will be using field "flow"
        denoting a positive or a negative variation in the flow. */

    // TODO
}

int main() {
    int o, d, to, td;
    vector<Vertex> G;
    /*  landings[i] stores an array of indices j
        for all j, G[j] is a destination vertex with airport = i
    */
    vector<vector<int>> landings(2);
    while (cin >> o >> d >> to >> td) {
        Vertex source = {o, to};
        Vertex dest = {d, td};
        G.push_back(source);
        G.push_back(dest);
        // edge from source to dest
        int sz = G.size();
        Edge e = {1, 1, sz-2, sz-1};
        G[sz - 2].adj.insert(e);
        // store dest vertex in the "landings" array
        if (landings.size() < max(o, d) + 1) landings.resize(max(o, d) + 1);
        landings[d].push_back(sz - 1);
    }

    // Iterate over all source-vertexs (those with even index)
    for (int i = 0; i < G.size(); i += 2) {
        for (int j : landings[G[i].airport]) {
            if (G[i].time - G[j].time >= 15) {
                Edge e = {0, 1, j, i};
                G[j].adj.insert(e);
            }
        }
    }


    // Source and sink
    Vertex s {-1, -1};
    Vertex t {-2, -2};
    G.push_back(s);
    G.push_back(t);
    int sz = G.size();
    for (int i = 0; i < sz - 2; i++) {
        if (i%2 == 0) {
            Edge e = {0, 1, sz-2, i};
            G[sz - 2].adj.insert(e);  // s --> G[i] (origin)
        }
        else {
            Edge e = {0, 1, i, sz-1};
            G[i].adj.insert(e);  // G[i] --> t (destination)
        }
    }

    /*
    cout << endl <<  "Test:" << endl << endl;

    for (Vertex v : G) {
        for (Edge e : v.adj) {
            cout << '[' << v.airport << " (" << v.time << ")] ----> [" << G[e.next].airport << "(" << G[e.next].time << ")]" << endl;
        }
    }
    cout << endl << endl;
    */

}
