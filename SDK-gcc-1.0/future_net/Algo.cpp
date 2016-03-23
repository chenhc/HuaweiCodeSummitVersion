#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <utility>
#include <queue>

using namespace std;

typedef pair<int, int> pair_i_i;

static int dp[1<<20][20];
static int route[1<<20][20];

static Route optimal_route;
static int yes = 0;
static int no = 0;


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
            if( !route._visit[G._Specified[i]])
            {
                no ++;
                return false;
            }
        }
        //yes ++;
        return true;
    }

    /*if(yes > 30 || no >100000000)
        return false;*/

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge[e]._dst;

        if(!route._visit[cur])
        {
            route._visit[cur] = 1;
            route._path.push_back(e);
            route._cost += G._Edge[e]._cost;

            if(dfs(G, cur, dst, route))  /* find a route*/
            {
                if(optimal_route._cost == 0)
                {
                    optimal_route = route;
                }
                else if(route._cost < optimal_route._cost)
                {
                    optimal_route = route; /* update the current best */
                }
            }

            route._cost -= G._Edge[e]._cost;
            route._path.pop_back();
            route._visit[cur] = 0;
        }
    }
    //no ++;
    return false;
}

void dfs_search_route(Graph &G)
{
    Route route;
    dfs(G, G._src, G._dst, route);
    if(optimal_route._cost < INF && optimal_route._cost != 0)
    {
        vector<int>::iterator it;
        for(it = optimal_route._path.begin(); it != optimal_route._path.end(); it++)
        {
            record_result(*it);
        }
    }
    printf("find a route, cost = %d\n", optimal_route._cost);
}

void cluster(Graph &G, DistMatrix dist, Component components[], int cluster_num)
{
    int included[nMAX] = {0};

    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;
    for(int k = 0; k < cluster_num; k++)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            /* choose a cluster head */
            if(!included[G._Specified[i]])
            {
                components[k]._head = G._Specified[i];
                components[k]._elems.push_back(G._Specified[i]);
                included[G._Specified[i]] = 1; /* mark the node has been clustered */

                /* add candidate partner for cluster head i*/
                for(int j = 0; j < G.specified_num; j++)
                {
                    if(!included[ G._Specified[j] ])
                        Q.push(make_pair(dist[ G._Specified[i] ][ G._Specified[j] ], G._Specified[j]));
                }
                /* add cluster member of i from candidate */
                int cnt = 0;
                while(!Q.empty() && cnt < 1)
                {
                    pair_i_i x = Q.top();
                    Q.pop();
                    components[k]._elems.push_back(x.second);
                    components[k]._tail = x.second;
                    included[x.second] = 1;
                    cnt ++;
                }

                /* cluster[k] finished */
                while(!Q.empty())
                    Q.pop();
                break;
            }
            else
                continue;
        }
    }

}

/* 局部dfs找路 */
int partial_dfs(Graph &G, int cur, int dst, vector<int> &route, int visit[nMAX], int radius)
{
    if(route.size() > radius)
        return false;
    if(cur == dst)
        return true;

    for(int e = G._first[cur]; e != -1; e = G._next[e])
    {
        cur = G._Edge[e]._dst;

        /* other cluster has not used this node to make connection,
            nor has  current cluster
        */
        if(!visit[cur])
        {
            visit[cur] = 1;
            route.push_back(e);

            if(partial_dfs(G, cur, dst, route, visit, radius))  /* find a route*/
                return true;

            route.pop_back();
            visit[cur] = 0;
        }
    }

    return false;
}

/* 使某个cluster分量内部连起来 */
int partial_connect(Graph &G, Component &component, int visit[nMAX])
{
    int i = component._elems.size();
    int src = component._elems[0], dst = component._elems[i-1];
    visit[src] = 1;
    int radius = 5;
    return partial_dfs(G, src, dst, component._path, visit, radius);

}

/* 各个cluster连接 */
int components_connect(Graph &G, Component &c_1, Component &c_2, vector<int> &bond, int visit[nMAX])
{
    int src = c_1._tail, dst = c_2._head;
    visit[src] = 1;
    visit[dst] = 0;
    return partial_dfs(G, src, dst, bond, visit, 5);
}

int fully_connect(Graph &G, Component components[], int cluster_num, int visit[nMAX])
{
    Route route_1, route_2;
    visit[G._src] = 1;
    if(partial_dfs(G, G._src, components[0]._head, route_1._path, visit, 5))
    {
        printf("起点到必经集合入口:\n");
        for(int i = 0; i < route_1._path.size(); i++)
            printf("%d->%d\n", G._Edge[route_1._path[i]]._src, G._Edge[route_1._path[i]]._dst);
    }

    visit[components[cluster_num-1]._tail] = 1;
    if(partial_dfs(G, components[cluster_num-1]._tail, G._src, route_2._path, visit, 30))
    {
        printf("必经集合出口到终点:\n");
        for(int i = 0; i < route_2._path.size(); i++)
            printf("%d->%d\n", G._Edge[route_2._path[i]]._src, G._Edge[route_2._path[i]]._dst);
    }
}

/* cluster 分类，分别处理，再连起来*/
int components_interconnect(Graph &G, DistMatrix dist, Component components[], int cluster_num, vector<int> bond[], int visit[nMAX])
{
    /* 各个cluster内部自连通 */
    for(int i = 0; i < cluster_num; i++)
    {
        if(partial_connect(G, components[i], visit))
        {
            printf("分量%d内部连接:\n", i);
            for(int j = 0; j < components[i]._path.size(); j++)
                printf("%d->%d\n", G._Edge[components[i]._path[j]]._src, G._Edge[components[i]._path[j]]._dst);
        }
    }

    /*
    for(int i = 0; i < cluster_num; i++)
        for(int j = 0; j < components[i]._path.size(); j++)
            printf("%d\n", components[i]._path[j]);
    */

    /* 分量互连 */
    for(int i = 0; i < cluster_num - 1; i++)
    {
        if(components_connect(G, components[i], components[i+1], bond[i], visit))
        {
            printf("分量%d 和 %d连接:\n", i, i+1);
            for(int j = 0; j < bond[i].size(); j++)
                printf("%d->%d\n", G._Edge[bond[i][j]]._src, G._Edge[bond[i][j]]._dst);
        }

    }

    return 0;
}

void divide_search_route(Graph &G)
{
    int cluster_num = G.specified_num/2 + (G.specified_num % 2 != 0 ? 1 : 0);
    Component components[15];
    vector<int> bond[15];
    int visit[nMAX] = {0};

    DistMatrix dist;
    Floyd(G, dist);
    /* 聚类 */
    cluster(G, dist, components, cluster_num);
    /* 全连接 */
    fully_connect(G, components, cluster_num, visit);

    /*分量互连*/
    components_interconnect(G, dist, components, cluster_num, bond, visit);

}
