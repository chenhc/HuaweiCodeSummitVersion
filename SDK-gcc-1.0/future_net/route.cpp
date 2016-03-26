#include "route.h"
#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include <stdio.h>


/*网络流数据结构*/
/*用于表示边的结构体，id，终点， 容量，费用，反向边*/
struct edge {int to, cap, cost, rev;};
int V;//顶点数
vector<edge> G[nMAX];//图的邻接表表示
int dis[nMAX];//最短距离
int pre_v[nMAX], pre_e[nMAX]; //最短路的前驱节点和对应边

void add_edge(int from, int to, int cap, int cost)
{
    G[from].push_back((edge){to, cap, cost, G[to].size()});
    G[to].push_back((edge){from, 0, -cost, G[from].size()-1});
}

/*求解从s到t流量为f的最小费用流*/
int min_cost_flow(int s, int t, int f)
{
    int res = 0;
    //Bellman-Ford s到t最短路
    while(f > 0) {
        fill(dis, dis + V, INF);
        dis[s] = 0;
        bool update = true;
        while(update) {
            update = false;
            for (int v = 0; v < V; v++) {
                if(dis[v] == INF) continue;
                for (int i = 0; i < G[v].size(); i++) {
                    edge &e = G[v][i];
                    if(e.cap > 0 && dis[e.to] > dis[v] + e.cost) {
                        dis[e.to] = dis[v] + e.cost;
                        pre_v[e.to] = v;
                        pre_e[e.to] = i;
                        update = true;
                    }
                }
            }
        }
    }
    //不能再增广了
    if(dis[t] == INF) return -1;

    //沿着s到t的路尽量增广
    int d = f;
    for (int v = t; v != s; v = pre_v[v]) {
        d = min(d, G[pre_v[v]][pre_e[v]].cap);
    }
    f = -d;
    res += d * dis[t];
    for (int v = t; v != s; v = pre_v[v]) {
        edge &e = G[pre_v[v]][pre_e[v]];
        e.cap -=  d;
        G[v][e.rev].cap += d;
    }
    return res;
}

int min_cost_flow_find_route(Graph &_G)
{
    //网络流模型建图
    int N = _G._nNum;
    for(int i = 0; i < _G._nNum; i++) {
        if(_G._must[i] || i == _G._src || i == _G._dst) {
            add_edge(i, i+N, 1, -INF);//i->i'
            for(int e = _G._first[i]; e != -1; e = _G._next[e]) {
                Link &link = _G._Edge[e];
                add_edge(i+N, link._dst, 1, link._cost);//i->j = i'->j
            }
        }
    }

    V = 2*_G._nNum;
    printf("from,to,cap.cost,rev\n");
    for(int v = 0; v < V; v++)
        for(vector<edge>::iterator it = G[v].begin(); it != G[v].end(); it++)
            printf("%d,%d,%d,%d,%d\n", v,(*it).to, (*it).cap, (*it).cost, (*it).rev);

    int s = _G._src;
    int t = _G._dst + N;
    int min_cost = min_cost_flow(s, t ,1);
    printf("min_cost = %d\n", min_cost);
}

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    Graph _G(topo, edge_num, demand);
    dfs_search_route(_G);

}
