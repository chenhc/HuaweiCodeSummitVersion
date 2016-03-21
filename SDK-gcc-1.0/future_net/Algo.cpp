#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"

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


