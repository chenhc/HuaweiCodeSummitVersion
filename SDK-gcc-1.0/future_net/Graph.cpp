#include "Graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

Graph::Graph(char *topo[],  int edge_num, char *demand)
{
    memset(_first, -1, sizeof(_first));
    memset(_pre_first, -1, sizeof(_pre_first));
    memset(_must, 0, sizeof(_must));

    int linkID, srcID, destID, cost, n_cnt = 0;
    for(int i = 0; i <edge_num; i++)
    {
       sscanf(topo[i], "%d,%d,%d,%d", &linkID, &srcID, &destID, &cost);
       _Edge[i]._linkID = linkID;
       _Edge[i]._src = srcID;
       _Edge[i]._dst = destID;
       _Edge[i]._cost = cost;

       _next[linkID] = _first[srcID];
       _first[srcID] = linkID;
       _pre_next[linkID] = _pre_first[destID];
       _pre_first[destID] = linkID;

       int temp = srcID>destID ? srcID : destID;
       n_cnt = n_cnt>temp ? n_cnt : temp;
    }
    _lNum = edge_num;
    _nNum = n_cnt + 1;

    char buf[100];
    sscanf(demand, "%d,%d,%s" , &_src, &_dst, buf);
    string _demand = string(buf);
    string::size_type i = 0;
    string::size_type j = _demand.find('|');
    int must_num = 0, id = -1;
    while(j != string::npos)
    {
        id = atoi(_demand.substr(i, j-i).c_str());
        _Specified[must_num++] = id;
        _must[id] = 1;
        i = ++ j;
        if(_demand.find('|', j) == string::npos)
        {
            j = _demand.find('|', j);
            _Specified[must_num++] = atoi(_demand.substr(i, j-i).c_str()) ;
            break;
        }
        j = _demand.find('|', j);
    }
    specified_num = must_num;
}

Route::Route()
{
    _cost = 0;
    memset(_visit, 0, sizeof(_visit));
}

