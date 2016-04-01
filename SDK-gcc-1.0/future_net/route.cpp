#include "route.h"
#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <vector>
#include <set>

extern int V, E;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    init_Graph(topo, edge_num, demand);
    if(V <= 100)
        Brute_Force::search_route();
    else
        SCC::search_route();
    //Brute_Force::search_route();
}



