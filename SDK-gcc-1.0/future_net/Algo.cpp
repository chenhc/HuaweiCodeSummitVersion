#include "Graph.h"
#include "Algo.h"
#include "lib/lib_record.h"
#include "string.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <queue>

using namespace std;

static Route optimal_route;
static DistMatrix dist;

static float bound;
time_t t_s, t_e;

typedef pair<int, int> pii;

typedef struct state
{
    int cur;
    int parent;
}State;

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

    /*if(G._nNum  <  30 &&  G._lNum < 120)
        bound = 999999;
    else{
        Floyd(G);
        bound = 0;
        for(int i = 0; i < G.specified_num; i++) {
            int v = G._Specified[i];
            int sum = dist[G._src][v] + dist[v][G._dst];
            if(sum > bound)
                bound = sum;
        }
        bound *= 2;
    }*/

    Floyd(G);
    priority_queue<pii, vector<pii>, greater<pii> > q;
    bool used[nMAX] = {0};
    int sum = 0;
    int cur = G._src;
    for(int i=0; i < G.specified_num; i++)
        q.push(make_pair(dist[cur][G._Specified[i]], G._Specified[i]));
    pii x = q.top();
    int next = x.second;
    sum += x.first;
    used[next] = 1;
    cur = next;
    while(!q.empty()) q.pop();
    for(;;) {
        int i = 0;
        for(int i=0; i<G.specified_num; i++)
            if(!used[G._Specified[i]])
                q.push(make_pair(dist[cur][G._Specified[i]], G._Specified[i]));
        if(q.empty()) break;
        pii x = q.top();
        next = x.second;
        sum += x.first;
        used[next] = 1;
        cur = next;
        while(!q.empty()) q.pop();
    }
    bound = 1.2*sum;
    printf("cut at %lf\n",  bound);
    if(bound > INF)
        return;

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

int bi_bfs(Graph &G, int src, int dst, Route &route)
{
    //距离矩阵
    for(int i = 0; i < G._nNum; i++)
        for(int j = 0; j < G._nNum; j++)
        {
            if(i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = INF;
        }

    //判断两点是否相邻
    for(int i = 0; i < G._lNum; i++)
        dist[G._Edge[i]._src][G._Edge[i]._dst] = G._Edge[i]._cost;

    vector<State> Q, rQ;
    int head[2] = {0, 0};
    int tail[2] = {-1, -1};
    Q.push_back((State){src, -1});
    tail[0]++;
    rQ.push_back((State){dst, -1});
    tail[1]++;
    for(;;) {
        //if(tail[0] - head[0] >= tail[1] - head[1]) {
        if(1) {
            //先扩展正向
            //正向扩展一层
            int _head = head[0], _tail = tail[0];
            for(int i = _head; i <= _tail; i++) {
                State &s = Q[i];
                //不断取当前状态的节点的相邻节点来扩展
                for(int j = G._first[s.cur]; j != -1; j = G._next[j]) {
                    Link &e = G._Edge[j];
                    //遍历反向队列，检查和反向有无相遇
                    for(int k = head[1]; k <= tail[1]; k++) {
                        State &rs = rQ[k];
                        if(e._dst == rs.cur)
                            return true;
                        //不是交点，正向入队
                        else {
                            Q.push_back((State){e._dst, i});
                            tail[0] ++;
                            break;
                        }
                    }
                }
                //队头元素处理完毕
                head[0] ++;
            }
            //反向扩展一层
            _head = head[1], _tail = tail[1];
            for(int i = _head; i <= _tail; i++) {
                State &s = rQ[i];
                //不断取当前状态的节点的相邻节点来扩展
                for(int j = G._pre_first[s.cur]; j != -1; j = G._pre_next[j]) {
                    Link &e = G._Edge[j];
                    //遍历正向队列，检查和正向有无相遇
                    for(int k = head[0]; k <= tail[0]; k++) {
                        State &rs = Q[k];
                        if(e._src == rs.cur)
                            return true;
                        //不是交点，正向入队
                        else {
                            rQ.push_back((State){e._src, i});
                            tail[1] ++;
                            break;
                        }
                    };
                }
                //队头元素处理完毕
                head[1] ++;
            }

        }//if
    }//for(;;)

}




