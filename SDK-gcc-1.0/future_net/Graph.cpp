#include "Graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

Graph::Graph(char *topo[],  int edge_num, char *demand)
{
    memset(_first, -1, sizeof(_first));
    memset(_pre_first, -1, sizeof(_pre_first));

    int linkID, srcID, destID, cost, n_cnt = 0;
    for(int i = 0; i <edge_num; i++)
    {
       sscanf(topo[i], "%d,%d,%d,%d", &linkID, &srcID, &destID, &cost);
       Link e(linkID, srcID, destID, cost);
       _Edge.push_back(e);

       _next[linkID] = _first[srcID];
       _first[srcID] = linkID;
       _pre_next[linkID] = _pre_first[destID];
       _pre_first[destID] = linkID;

       int temp = srcID>destID? srcID : destID;
       n_cnt = n_cnt>temp ? n_cnt : temp;
    }
    _lNum = edge_num;
    _nNum = n_cnt + 1;

    char buf[100];
    sscanf(demand, "%d,%d,%s" , &_src, &_dst, buf);
    string _demand = string(buf);
    string::size_type i = 0;
    string::size_type j = _demand.find('|');
    while(j != string::npos)
    {
        _Specified.insert( atoi(_demand.substr(i, j-i).c_str() ) );
        i = ++ j;
        j = _demand.find('|', j);
    }
    specified_num = _Specified.size();
}

Route::Route()
{
    memset(_visit, 0, sizeof(_visit));
}

void Route::add(const Graph &G, int e)
{
    int node = G._Edge.at(e)._dst;
    _path.push_back(e);
    _visit[node] = 1;

    /* if be a specified node */
    if(G._Specified.count(node))
        _already.push_back(node);
}

void Route::rm(const Graph &G, int e)
{
    int node =  G._Edge.at(e)._dst;
    _path.pop_back();
    _visit[node] = 0;

    /* if be a specified node */
    if(G._Specified.count(node))
        _already.pop_back();
}

void Route::print(const Graph &G)
{
    for(int i = 0; i < _path.size(); i++)
    {
        Link L = G._Edge[_path[i]];
        printf("%d -> %d\n", L._src, L._dst);
    }
}
