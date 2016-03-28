#include "Graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

Graph::Graph(char *topo[],  int edge_num, char *demand)
{
    memset(first, -1, sizeof(first));
    memset(pre_first, -1, sizeof(pre_first));
    memset(must, 0, sizeof(must));

    int linkID, srcID, destID, cost, n_cnt = 0;
    for(int i = 0; i <edge_num; i++)
    {
       sscanf(topo[i], "%d,%d,%d,%d", &linkID, &srcID, &destID, &cost);
       Edge[i].linkID = linkID;
       Edge[i].src = srcID;
       Edge[i].dst = destID;
       Edge[i].cost = cost;

       next[linkID] = first[srcID];
       first[srcID] = linkID;
       pre_next[linkID] = pre_first[destID];
       pre_first[destID] = linkID;

       int temp = srcID > destID ? srcID : destID;
       n_cnt = n_cnt > temp ? n_cnt : temp;
    }
    lNum = edge_num;
    nNum = n_cnt + 1;

    char buf[100];
    sscanf(demand, "%d,%d,%s" , &src, &dst, buf);
    string _demand = string(buf);
    string::size_type i = 0;
    string::size_type j = _demand.find('|');
    int must_num = 0, id = -1;
    while(j != string::npos)
    {
        id = atoi(_demand.substr(i, j-i).c_str());
        Specified[must_num++] = id;
        must[id] = 1;
        i = ++ j;
        if(_demand.find('|', j) == string::npos)
        {
            j = _demand.find('|', j);
            id = atoi(_demand.substr(i, j-i).c_str());
            Specified[must_num++] = id;
            must[id] = 1;
            break;
        }
        j = _demand.find('|', j);
    }
    specified_num = must_num;
}

Route::Route()
{
    cost = 0;
    memset(visit, 0, sizeof(visit));
}

