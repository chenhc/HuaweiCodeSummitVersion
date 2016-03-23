#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>
#include <queue>
#include <utility>

using namespace std;

typedef pair<int, int> pair_i_i;

static int dp[1<<20][20];
static int route[1<<20][20];

static Route optimal_route;

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

int dfs(Graph &G, int cur, int dst, Route &route, int range)
{
    if(route._cost > range)
        return false;

    if(cur == dst)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            if( !route._visit[G._Specified[i]])
            {
                return false;
            }
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
            route._cost += G._Edge[e]._cost;

            if(dfs(G, cur, dst, route, range))  /* find a route*/
            {
                if(optimal_route._cost == 0)
                {
                    optimal_route = route;
                }
                else if(route._cost < optimal_route._cost)
                {
                    optimal_route = route; /* update the current best */
                }
                return true;
            }

            route._cost -= G._Edge[e]._cost;
            route._path.pop_back();
            route._visit[cur] = 0;
        }
    }
    return false;
}

void dfs_search_route(Graph &G)
{
    Route route;
    route._visit[G._src] = 1;
    dfs(G, G._src, G._dst, route);
    if(optimal_route._cost < INF && optimal_route._cost != 0)
    {
        vector<int>::iterator it;
        for(it = optimal_route._path.begin(); it != optimal_route._path.end(); it++)
        {
            record_result(*it);
        }
    }
    //printf("find a route, cost = %d\n", optimal_route._cost);
}

int dfs_2(Graph &G, int cur, int dst, Route &route, int range)
{
    if(route._cost > range)
        return false;

    if(cur == dst)
        return true;

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge[e]._dst;

        if(!route._visit[cur])
        {
            route._visit[cur] = 1;
            route._path.push_back(e);
            route._cost += G._Edge[e]._cost;

            if(dfs_2(G, cur, dst, route, range))  /* find a route*/
                return true;

            route._cost -= G._Edge[e]._cost;
            route._path.pop_back();
            route._visit[cur] = 0;
        }
    }
    return false;
}

int greedy_dfs(Graph &G, DistMatrix dist, int cur, int dst, Route &route, int range)
{
    if(route._cost > range)
        return false;

    if(cur == dst)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            if( !route._visit[G._Specified[i]])
            {
                return false;
            }
        }
        return true;
    }

    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;
    for(int e = G._first[cur]; e != -1; e = G._next[e])
        Q.push(make_pair(dist[cur][G._Edge[e]._dst], e));

    while(!Q.empty())
    {
        pair_i_i x = Q.top();
        Q.pop();
        int e = x.second;
        cur = G._Edge[e]._dst;

        if(!route._visit[cur])
        {
            route._visit[cur] = 1;
            route._path.push_back(e);
            route._cost += G._Edge[e]._cost;

            if(greedy_dfs(G, dist, cur, dst, route))  /* find a route*/
                return true;

            route._cost -= G._Edge[e]._cost;
            route._path.pop_back();
            route._visit[cur] = 0;
        }
    }
    return false;
}

int greedy_dfs_for_dst(Graph &G, DistMatrix dist, int cur, int dst, Route &route, int range)
{
    if(route._cost > range)
        return false;

    if(cur == dst)
        return true;


    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;
    for(int e = G._first[cur]; e != -1; e = G._next[e])
        Q.push(make_pair(dist[G._Edge[e]._dst][G._dst], e));

    while(!Q.empty())
    {
        pair_i_i x = Q.top();
        Q.pop();
        int e = x.second;
        cur = G._Edge[e]._dst;

        if(!route._visit[cur])
        {
            route._visit[cur] = 1;
            route._path.push_back(e);
            route._cost += G._Edge[e]._cost;

            if(greedy_dfs(G, dist, cur, dst, route))  /* find a route */
                return true;

            route._cost -= G._Edge[e]._cost;
            route._path.pop_back();
            route._visit[cur] = 0;
        }
    }

    return false;

}

void dfs_search_route_2(Graph &G)
{
    DistMatrix dist;
    Floyd(G, dist);

    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;
    priority_queue<pair_i_i, vector<pair_i_i>, less<pair_i_i> > rQ;
    for(int i = 0; i < G.specified_num; i++)
    {
        Q.push(make_pair(dist[G._src][G._Specified[i]], G._Specified[i] ));
        rQ.push(make_pair(dist[G._src][G._Specified[i]], G._Specified[i] ));
    }

    pair_i_i x = Q.top();
    int d = x.first;
    int u = x.second;

    Route route;
    int index_1 = 0, index_2 = 0;
    route._visit[G._src] = 1;
    route._visit[G._dst] = 1;
    if(dfs_2(G, G._src, u, route, d))
    {
        printf("step 1:\n");
        index_1 =  route._path.size();
        printf("%d\n", index_1);
        vector<int>::iterator it;
        for(it = route._path.begin(); it != route._path.end(); it++)
        {
            printf("%d->%d\n", G._Edge[*it]._src, G._Edge[*it]._dst);
        }
    }

    for(int i = 0; i < G._nNum; i++)
    {
        int cnt = 0;
        for(int e = G._first[i]; e != -1; e = G._next[e])
            if( G._must[G._Edge[e]._dst] )
                cnt --;

        if(cnt < 0)
            for(int e = G._pre_first[i]; e != -1; e = G._pre_next[e])
                dist[G._Edge[e]._src][i] = cnt;
    }

    for(int i = 0; i < G._nNum; i++)
    {
        int cnt = 0;
        for(int e = G._first[i]; e != -1; e = G._next[e])
            if( dist[i][G._Edge[e]._dst] < 0)
                cnt --;

        if(cnt < 0)
            for(int e = G._pre_first[i]; e != -1; e = G._pre_next[e])
                dist[G._Edge[e]._src][i] = cnt;
    }

    while(!rQ.empty())
    {
        pair_i_i x = rQ.top();
        rQ.pop();
        int v = x.second;
        if(!route._visit[v])
        {
            if(greedy_dfs(G, dist, u, v, route))
            {
                printf("step 2:\n");
                index_2 =  route._path.size();
                printf("%d\n", index_2);
                vector<int>::iterator it;
                for(it = route._path.begin(); it != route._path.end(); it++)
                {
                    printf("%d->%d\n", G._Edge[*it]._src, G._Edge[*it]._dst);
                }
            }
        }
    }


    vector<int>::iterator it = route._path.end();
    int cur = G._Edge[*(it - 1)]._dst;
    route._visit[G._dst] = 0;

    if(greedy_dfs_for_dst(G, dist, cur, G._dst, route))
    {
        printf("step 3:\n");
        vector<int>::iterator it;
        for(it = route._path.begin(); it != route._path.end(); it++)
        {
            printf("%d->%d\n", G._Edge[*it]._src, G._Edge[*it]._dst);
        }
        printf("success!\n");
    }

}

