#ifndef METHOD_H_INCLUDED
#define METHOD_H_INCLUDED

#include <vector>

#define MAX_V 600
#define INF 999999

typedef struct Edge
{
    int id;
    int to;
    int cost;
}Edge;

typedef struct Trace
{
    int pre; //前驱节点
    int reverse_e; //反向回路
}Trace;

typedef int DistMatrix[MAX_V][MAX_V];

void add_edge(int id, int from, int to, int cost);

void init_Graph(char *topo[5000], int edge_num, char *demand);

void floyd();

namespace SCC {
    void dfs(int v);
    void rdfs(int v, int k);
    int scc();
    bool dfs_search_route(int cur);
    void search_route();
}

namespace Brute_Force {
    bool dfs(int cur);
    void search_route();
}

namespace Heuristic {

    void bfs();
}

int random(int x, int y);

void Dijkstra(int src, int dst, std::vector<Edge> G[MAX_V], Trace path[MAX_V]);

#endif // METHOD_H_INCLUDED
