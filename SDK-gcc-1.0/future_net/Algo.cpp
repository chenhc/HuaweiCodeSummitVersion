#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>
#include <time.h>
#include <queue>
#include <stack>

using namespace std;

static Route optimal_route;
static DistMatrix dist;

static float bound;
static int start, stop;

typedef pair<int, int> pair_i_i;

void Floyd(Graph &G)
{
    /*initialize distence matrix*/
    int node_num = G.nNum;
    for(int i = 0; i < node_num; i++)
        for(int j = 0; j < node_num; j++)
        {
            if(i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = INF;
        }

    /* weight of each edge counts */
    for(int i = 0; i < G.lNum; i++)
        dist[G.Edge[i].src][G.Edge[i].dst] = G.Edge[i].cost;

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
            if( !route.visit[G.Specified[i]])
                return false;
        }
        return true;
    }

    for(int e = G.first[cur]; e != -1; e = G.next[e])
    {
        cur = G.Edge[e].dst;

        if(!route.visit[cur])
        {
            route.visit[cur] = 1;
            route.path.push_back(e);
            route.cost += G.Edge[e].cost;

            if(dfs(G, cur, dst, route))  /* find a route*/
                return true;

            route.cost -= G.Edge[e].cost;
            route.path.pop_back();
            route.visit[cur] = 0;
        }
    }
    return false;
}

int dfs(Graph &G, int center, int radiu, bool visit[lMAX])
{
    if(radiu == 0)
        return true;

    for(int e = G.first[center]; e != -1; e = G.next[e]) {
        if(!visit[e]){
            visit[e] = 1;
            Link &edge = G.Edge[e];
            edge.cost = -INF;
            dfs(G, edge.dst, radiu-1, visit);
        }
    }
}

int greedy_dfs(Graph &G, int cur, int dst, Route &route)
{
    if(cur == dst)
    {
        for(int i = 0; i < G.specified_num; i++)
        {
            if( !route.visit[G.Specified[i]])
                return false;
        }
        return true;
    }

    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;
    for(int e = G.first[cur]; e != -1; e = G.next[e])
        Q.push(make_pair(G.Edge[e].cost, e));

    while(!Q.empty())
    {
        pair_i_i x = Q.top();
        Q.pop();

        int e = x.second;
        cur = G.Edge[e].dst;

        if(!route.visit[cur])
        {
            route.visit[cur] = 1;
            route.path.push_back(e);
            route.cost += G.Edge[e].cost;

            if(dfs(G, cur, dst, route))  /* find a route*/
                return true;

            route.cost -= G.Edge[e].cost;
            route.path.pop_back();
            route.visit[cur] = 0;
        }
    }
    return false;
}

int greedy_search_route(Graph &G)
{
    Graph _G(G);

    bool visit[lMAX];
    for(int i = 0; i < G.specified_num; i++) {
        int v = G.Specified[i];
        memset(visit, 0, sizeof(visit));
        dfs(_G, v, 3, visit);
    }

    Route route;
    if(greedy_dfs(_G, _G.src, _G.dst, route)) {
        int cost = 0;
        vector<int>::iterator it;
        printf("src=%d dst=%d\n", G.src, G.dst);
        for(it = route.path.begin(); it != route.path.end(); it++)
        {
            Link &e = G.Edge[*it];
            cost += e.cost;
            printf("%d->%d\n", e.src, e.dst);
        }
        printf("find a route, cost = %d\n", cost);
    }

}


void dfs_search_route(Graph &G)
{
    start = clock();
    Route route;

    dfs(G, G.src, G.dst, route);
    printf("find a route, cost = %d\n", route.cost);

    if(optimal_route.cost < INF && optimal_route.cost != 0)
    {
        vector<int>::iterator it;
        for(it = optimal_route.path.begin(); it != optimal_route.path.end(); it++)
        {
            record_result(*it);
        }
    }
    printf("find a route, optimal_cost = %d\n", optimal_route.cost);
    stop = clock();
}

void Dijkstra(Graph &G, int src, int D[nMAX], int path[nMAX]) /*保存路径,path[nodeID] = edgeID*/
{
    memset(path, -1, sizeof(path));
    /* 最小值优先的优先队列 */

    /*标号数组*/
    for(int i=0; i<G.nNum; i++)
        D[i] = (i==src ? 0: INF);

    /*优先队列*/
    priority_queue<pair_i_i, vector<pair_i_i>, greater<pair_i_i> > Q;

    /*起点进入优先队列*/
    Q.push(make_pair(D[src], src));
    while(!Q.empty())
    {
        /*队列中具有最小标号的*/
        pair_i_i x = Q.top();
        Q.pop();
        int u = x.second;
        /* 避免节点的重复处理 */
        if(x.first != D[u]) continue;
        //if( u == dst) /*到达终点*/
            //break;

        /*更新节点u出发的所有边 */
        for(int e = G.first[u]; e != -1; e = G.next[e])
        {
            int v = G.Edge[e].dst;
            if(D[u] + G.Edge[e].cost < D[v])
            {
                D[v] = D[u] + G.Edge[e].cost;
                Q.push(make_pair(D[v], v)); /*标号更新成功，加入优先队列*/
                path[v] = e; /*更新反向路径*/
            }
        }

    }

    //保存子路径
    /*int v = dst;
    stack<int> _path;
    while(v != src) {
        route.visit[v] = 1;
        _path.push(path[v]);
        v = G.Edge[path[v]].src;
    }
    while(!_path.empty()) {
        route.path.push_back(_path.top());
        _path.pop();
    }*/
}





