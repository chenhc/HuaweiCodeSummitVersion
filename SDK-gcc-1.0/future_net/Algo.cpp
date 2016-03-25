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
                return false;
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
    printf("find a route, optimal_cost = %d\n", optimal_route._cost);
}


//求网络最小费用最大流,邻接阵形式
//返回最大流量,flow返回每条边的流量,netcost返回总费用
//传入网络节点数n,容量mat,单位费用cost,源点source,汇点sink

int min_cost_max_flow(int n, int mat[][nMAX], int cost[][nMAX], int source, int sink, int flow[][nMAX], int& netcost)
{
	int pre[nMAX], min[nMAX], d[nMAX], i, j, t, tag;
	if (source == sink)
        return INF;
	for (i=0; i<n; i++)
		for (j=0; j<n; flow[i][j++]=0);

	for (netcost=0;;){
        for (i=0;i<n;i++)
            pre[i]=0,min[i]=INF;
		//通过bellman_ford寻找最短路，即最小费用可改进路
		for (pre[source]=source+1,min[source]=0,d[source]=INF,tag=1;tag;)
			for (tag=t=0;t<n;t++)
				if (d[t])
					for (i=0;i<n;i++)
						if (j=mat[t][i]-flow[t][i]&&min[t]+cost[t][i]<min[i])
							tag=1,min[i]=min[t]+cost[t][i],pre[i]=t+1,d[i]=d[t]<j?d[t]:j;
						else if (j=flow[i][t]&&min[t]<INF&&min[t]-cost[i][t]<min[i])
							tag=1,min[i]=min[t]-cost[i][t],pre[i]=-t-1,d[i]=d[t]<j?d[t]:j;
		if (!pre[sink])	break;
		for (netcost+=min[sink]*d[i=sink];i!=source;)
			if (pre[i]>0)
				flow[pre[i]-1][i]+=d[sink],i=pre[i]-1;
			else
				flow[i][-pre[i]-1]-=d[sink],i=-pre[i]-1;
	}
	for (j=i=0;i<n;j+=flow[source][i++]);
	return j;
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
    int net_flow = min_cost_max_flow(2*G._nNum, mat, cost, G._src, G._dst+G._nNum, flow, net_cost);

    printf("net_cost = %d\nnet_flow = %d\n", net_cost, net_flow);

    //for(int i=0; i<0)

}



