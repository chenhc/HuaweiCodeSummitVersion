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

    set<int>::iterator it;
    for(it = G._Specified.begin(); it != G._Specified.end(); it++)
    {
        _demand = _demand | (1<<(*it));
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
        record_result(G._Edge[e]._linkID);
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
    vector<Link>::const_iterator it = G._Edge.begin();
    for( ; it != G._Edge.end(); it ++)
        dist[(*it)._src][(*it)._dst] = (*it)._cost;

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

int dfs(Graph &G, int cur, int dst, Route &path)
{
    if(cur == dst)
    {
        return path._already.size() == G.specified_num;
    }

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge.at(e)._dst;

        if(!path._visit[cur])
        {
            path.add(G, e);

            if(dfs(G, cur, dst, path))
                return true;

            path.rm(G, e);
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

int enhenced_dfs(Graph &G, DistMatrix dist, int *label, int cur, int dst, Route &path)
{
    if(cur == dst)
    {
        return path._already.size() == G.specified_num;
    }

    vector<int> remain(60);
    vector<int>::iterator end_it = set_difference(G._Specified.begin(), G._Specified.end(),
                                                  path._already.begin(), path._already.end(),
                                                  remain.begin());

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge.at(e)._dst;

        /*剪枝*/
        int ok = 1;
        if(label[cur] == 1) /*坏点*/
        {
            vector<int>::iterator it;
            for(it = remain.begin(); it != end_it; it ++)
            {
                if(dist[cur][*it] >= INF)
                {
                    ok = 0;
                    break;
                }
            }
        }

        if(ok && !path._visit[cur])
        {
            path.add(G, e);

            if(enhenced_dfs(G, dist, label, cur, dst, path))
                return true;

            path.rm(G, e);
        }
    }

    return false;

}

void enhenced_dfs_search_route(Graph &G)
{
    DistMatrix dist;
    Floyd(G, dist);
    int bad[nMAX] = {0};

    /* 到必经点没有通路的点，被标记为bad */
    for(int i = 0; i < G._nNum; i++)
    {
        for(set<int>::iterator it = G._Specified.begin(); it != G._Specified.end(); it++)
        {
            if(dist[i][*it] >= INF)
            {
                bad[i] = 1;
                break;
            }
        }
    }

    Route route;
    if(enhenced_dfs(G, dist, bad, G._src, G._dst, route))
    {
        vector<int>::iterator it;
        for(it = route._path.begin(); it != route._path.end(); it++)
        {
            record_result(*it);
        }
    }

}
