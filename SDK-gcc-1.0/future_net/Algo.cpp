#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

static Route optimal_route;
static DistMatrix dist;
static DistMatrix r_dist;
static float bound;
time_t t_s, t_e;

void Floyd(Graph &G)
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

    /* rfloyd*/
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
    t_e = time(NULL);
    if(difftime(t_e, t_s) > 8)
        return false;

    if (route._cost + dist[cur][dst] > bound)
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
                    bound = route._cost;
                }
                else if(route._cost < optimal_route._cost)
                {
                    optimal_route = route; /* update the current best */
                    bound =  route._cost;
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
    t_s = time(NULL);
    Route route;

    if(G._nNum  <  30 &&  G._lNum < 120)
        bound = 999999;
    else{
        Floyd(G);
        bound = 0;
      for(int i = 0; i < G.specified_num; i++)
        {
            int v = G._Specified[i];
            int sum = dist[G._src][v] + r_dist[G._dst][v];
            if(sum > bound)
                bound = sum;
        }
        bound *= 2;        
    }
    printf("cut at %lf\n",  bound);

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
    printf("time = %lf\n", difftime(t_e, t_s));
}




