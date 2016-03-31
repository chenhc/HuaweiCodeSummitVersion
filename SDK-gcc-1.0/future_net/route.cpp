#include "route.h"
#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <vector>
#include <set>



//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    init_Graph(topo, edge_num, demand);
    SCC::search_route();
}



