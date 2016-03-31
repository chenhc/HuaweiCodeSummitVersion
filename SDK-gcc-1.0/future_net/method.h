#ifndef METHOD_H_INCLUDED
#define METHOD_H_INCLUDED

void add_edge(int id, int from, int to, int cost);

void init_Graph(char *topo[5000], int edge_num, char *demand);

namespace SCC {
    void dfs(int v);
    void rdfs(int v, int k);
    int scc();
    bool dfs_search_route(int cur, int edge = -1);
    void search_route();
}


#endif // METHOD_H_INCLUDED
