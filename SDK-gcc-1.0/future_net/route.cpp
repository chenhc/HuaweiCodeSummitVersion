#include "route.h"
#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <vector>
#include <set>



//��Ҫ��ɵĹ��������
void search_route(char *topo[5000], int edge_num, char *demand)
{
    init_Graph(topo, edge_num, demand);
    SCC::search_route();
}



