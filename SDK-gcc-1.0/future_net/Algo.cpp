#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <utility>
#include <algorithm>


using namespace std;
static int dp[1<<20][20];
static int route[1<<20][20];


int dp_search(Graph &G, int S, int v, int demand)
{
    if(dp[S][v] >= 0)
        return dp[S][v];
    if((S & demand) == demand && v == G._dst)
        return dp[S][v] = 0;

    int res = INF;
    for(int e = G._first[v]; e != -1; e = G._next[e])
    {
        int u = G._Edge[e]._dst;
        if(!(S>>u & 1))
        {
            int temp = G._Edge[e]._cost + dp_search(G, S|1<<u, u, demand);
            if(temp < res)
            {
                res = temp;
                route[S][v] = e;
            }
        }
    }

    return dp[S][v] = res;
}

void dp_search_route(Graph &G)
{
    memset(dp, -1, sizeof(dp));

    int _demand = 0;

    for(int i = 0; i < G.specified_num; i++)
    {
        _demand = _demand | (1<<i);
    }
    _demand = _demand | (1<<G._dst);

    int result = dp_search(G, 1<<G._src, G._src, _demand);
    if(result >= INF)
        return;

    int u = G._src, S = 1<<G._src, e = -1;
    while(u != G._dst)
    {
        e = route[S][u];
        u = G._Edge[e]._dst;
        S = S | (1<<u);
        record_result(e);
    }
}

void Floyd(Graph &G, DistMatrix dist)
{
    /*initialize distence matrix*/
    int node_num = G._nNum;
    for(int i = 0; i < node_num; i++)
        for(int j = 0; j < node_num; j++)
        {
            if(i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = INF;
        }

    /* weight of each edge counts */
    for(int i = 0; i < G._lNum; i++)
        dist[G._Edge[i]._src][G._Edge[i]._dst] = G._Edge[i]._cost;

    /* floyd */
    for(int k = 0; k < node_num; k++)
        for(int i = 0; i < node_num; i++)
            for(int j = 0; j < node_num; j++)
            {
                int temp = dist[i][k] + dist[k][j];
                if(temp < dist[i][j])
                    dist[i][j] = temp;
            }
}

int dfs(Graph &G, int cur, int dst, Route &route)
{
    if(cur == dst)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            if( !route._visit[G._Specified[i]]) return false;
        }
        return true;
    }

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge[e]._dst;

        if(!route._visit[cur])
        {
            route._visit[cur] = 1;
            route._path.push_back(e);

            if(dfs(G, cur, dst, route))
                return true;

            route._path.pop_back();
            route._visit[cur] = 0;

        }
    }

    return false;
}

void dfs_search_route(Graph &G)
{
    Route route;
    if(dfs(G, G._src, G._dst, route))
    {
        vector<int>::iterator it;
        for(it = route._path.begin(); it != route._path.end(); it++)
        {
            record_result(*it);
        }
    }
}
