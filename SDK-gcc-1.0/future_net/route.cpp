#include "route.h"
#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include <stdio.h>

//��Ҫ��ɵĹ��������
void search_route(char *topo[5000], int edge_num, char *demand)
{
    Graph G(topo, edge_num, demand);
    //dfs_search_route(G);
    greedy_search_route(G);
    //Route route;
    //bi_bfs(G, G._src, G._dst, route);
}
