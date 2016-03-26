#include "route.h"
#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include <stdio.h>

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    Graph _G(topo, edge_num, demand);
    dfs_search_route(_G);
}
