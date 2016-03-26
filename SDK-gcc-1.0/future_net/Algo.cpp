#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>

#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>


using namespace std;

static Route optimal_route;
static DistMatrix dist;
static DistMatrix r_dist;
static int up_bound;

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


void r_Floyd(Graph &G, DistMatrix r_dist)
{
    /*initialize distence matrix*/
    int node_num = G._nNum;
    for(int i = 0; i < node_num; i++)
        for(int j = 0; j < node_num; j++)
        {
            if(i == j)
                r_dist[i][j] = 0;
            else
                r_dist[i][j] = INF;
        }

    /* weight of each edge counts */
    for(int i = 0; i < G._lNum; i++)
        r_dist[G._Edge[i]._dst][G._Edge[i]._src]= G._Edge[i]._cost;

    /* floyd */
    for(int k = 0; k < node_num; k++)
        for(int i = 0; i < node_num; i++)
            for(int j = 0; j < node_num; j++)
            {
                int temp = r_dist[i][k] + r_dist[k][j];
                if(temp < r_dist[i][j])
                    r_dist[i][j] = temp;
            }
}

int dfs(Graph &G, int cur, int dst, Route &route)
{
    if(cur == dst)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            if( !route._visit[G._Specified[i]])
                return false;
        }
        return true;
    }

    if(route._cost + dist[cur][dst] > up_bound)
       return false;


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
                    up_bound = 0.9*route._cost;
                }
                else if(route._cost < optimal_route._cost)
                {
                    optimal_route = route; /* update the current best */
                    up_bound = 0.9*route._cost;
                }
                //return true;
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

    Floyd(G, dist);
    r_Floyd(G, r_dist);

    up_bound = 0;
    for(int i = 0; i < G.specified_num; i++)
    {
        int v = G._Specified[i];
        int sum = dist[G._src][v] + r_dist[G._dst][v];
        if(sum > up_bound)
            up_bound = sum;
    }
    up_bound *= 2;


    printf("cut at %d\n", up_bound);

    dfs(G, G._src, G._dst, route);
    if(optimal_route._cost < INF && optimal_route._cost != 0)
    {
        vector<int>::iterator it;
        for(it = optimal_route._path.begin(); it != optimal_route._path.end(); it++)
        {
            record_result(*it);
        }
    }
    printf("find a route, optimal_cost = %d\n", optimal_route._cost);
}




void min_cost_max_flow_find_route(Graph &G)
{
    /*建邻接矩阵*/
    DistMatrix d;
    for(int i=0; i<G._nNum; i++)
        for(int j=0; j<G._nNum; j++)
            d[i][j] = (i == j? 0 : INF);
    /*原图权值矩阵*/
    for(int e=0; e<G._lNum; e++)
        d[G._Edge[e]._src][G._Edge[e]._dst] = G._Edge[e]._cost;

    /* 带宽矩阵，花费矩阵，流量矩阵（待求的流量分布）*/
    DistMatrix mat, cost, flow;

    for(int i=0; i < 2*G._nNum; i++)
        for(int j=0; j < 2*G._nNum; j++) {
            mat[i][j] = 1;  //全部边为单位流量
            flow[i][j] = 0; //初始流量分布为0
            cost[i][j] = INF; //初始花费为INF
        }
    /* 原图转化 */
    //映射 i'=i+N
    for(int i=0; i < G._nNum; i++){
        cost[i][i] = 0;
        if(G._must[i] || i == G._src || i == G._dst){
            cost[i][i+G._nNum] = -INF; //cost[i][i']=inf,使其必经
            cost[i+G._nNum][i] = INF;
        }
    }

    for(int i = 0; i < G.specified_num; i++)
    {
        int s = G._Specified[i];
        int _s = s + G._nNum;
        for(int j = 0; j < G._nNum; j++)
        {
            if(s != j)
                cost[_s][j] = d[s][j]; //cost[i'][j] = d[i][j]，承包原像的出口
        }
    }

    int net_cost = 0;
    int net_flow;//= min_cost_max_flow(2*G._nNum, mat, cost, G._src, G._dst+G._nNum, flow, net_cost);

    printf("net_cost = %d\nnet_flow = %d\n", net_cost, net_flow);


}



