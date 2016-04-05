#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <stdlib.h>
#include <string> //string
#include <set>
#include <vector>
#include <stack>
#include <map>
#include <utility>
#include <queue>
#include <string.h> //memset
#include <time.h>

typedef std::pair<int, int> pair_i_i;

std::vector<Edge> G[MAX_V]; //图的邻接表
std::vector<Edge> rG[MAX_V]; //反向边邻接表

std::vector<int> PostOrder; //scc第一次dfs过程中的后序存放数组
bool used[MAX_V]; //scc在dfs过程中的访问标记数组
int cmp[MAX_V]; //拓扑序

int src, dst; //起点，终点
int V = 0, E = 0; //顶点数，边数
std::set<int> neccesity; //必经点集合
std::set<int> already; //已访问必经点
std::set<int> remain; //待访问必经点
bool isMust[MAX_V]; //是否为必经点

std::vector<int> v_path; //节点表示的路径
std::vector<int> e_path; //边表示的路径
bool visit[MAX_V]; //已访问节点标记
std::vector<int> optimal_e_path;
std::vector<int> optimal_v_path;
int total_cost = 0;
int optimal_cost = INF;

int start, stop;

DistMatrix dist; //距离矩阵

bool bfsVisited[MAX_V];

//主要函数实现
//添加边
void add_edge(int id, int from, int to, int cost)
{
    G[from].push_back((Edge){id, to, cost});
    rG[to].push_back((Edge){id, from, cost});
}

//读取拓扑数据
void init_Graph(char *topo[5000], int edge_num, char *demand)
{
    int id, from, to, cost;
    for (int i = 0; i < edge_num; i++){
        sscanf(topo[i], "%d,%d,%d,%d", &id, &from, &to, &cost);
        add_edge(id, from, to, cost);
        int temp = from > to ? from : to;
        V = V > temp ? V : temp;
    }
    V ++;
    E = edge_num;

    memset(isMust, 0, sizeof(isMust));
    char buf[100];
    sscanf(demand, "%d,%d,%s" , &src, &dst, buf);
    std::string _demand = std::string(buf);
    std::string::size_type i = 0;
    std::string::size_type j = _demand.find('|');
    while(j != std::string::npos)
    {
        int id = atoi(_demand.substr(i, j-i).c_str());
        neccesity.insert(id);
        remain.insert(id);
        isMust[id] = true;
        i = ++ j;
        if(_demand.find('|', j) == std::string::npos)
        {
            j = _demand.find('|', j);
            id = atoi(_demand.substr(i, j-i).c_str());
            neccesity.insert(id);
            remain.insert(id);
            isMust[id] = true;
            break;
        }
        j = _demand.find('|', j);
    }

}

//scc的dfs操作
void SCC::dfs(int v)
{
    used[v] = true;
    for(int i = 0; i < G[v].size(); i++)
    {
        Edge &e = G[v][i];
        if(!used[e.to])
            dfs(e.to);
    }
    PostOrder.push_back(v);
}

void SCC::rdfs(int v, int k)
{
    used[v] = true;
    cmp[v] = k;
    for(int i = 0; i < rG[v].size(); i++)
    {
        Edge &e = rG[v][i];
        if(!used[e.to])
            rdfs(e.to, k);
    }
}

int SCC::scc()
{
    memcpy(used, visit, sizeof(used));
    PostOrder.clear();
    for(int v = 0; v < V; v++) {
        if(!used[v] && !visit[v]) dfs(v);
    }

    memcpy(used, visit, sizeof(used));
    memset(cmp, -1, sizeof(cmp));
    int k = 0;
    for(int i = PostOrder.size() - 1; i >= 0; i--) {
        if(!used[PostOrder[i]]) {
            rdfs(PostOrder[i], k++);
        }
    }
    return k;
}

bool SCC::dfs_search_route(int cur)
{
    //当前边到达终点
    if(cur == dst) {
        for(std::set<int>::iterator it = neccesity.begin(); it != neccesity.end(); it++)
            if(!visit[*it])
                return false;

        //当前边到达终点，且经过所有必经点
        if(optimal_cost >= INF) {
            optimal_cost = total_cost;
            optimal_e_path.assign(e_path.begin(), e_path.end());
            optimal_v_path.assign(v_path.begin(), v_path.end());
        }
        else if (total_cost < optimal_cost) {
            optimal_cost = total_cost;
            optimal_e_path.assign(e_path.begin(), e_path.end());
            optimal_v_path.assign(v_path.begin(), v_path.end());
        }
        return true;
    }

    //超时强制退出
    stop = clock();
    if((double)(stop - start)/CLOCKS_PER_SEC > 8.5)
        return false;

    //强连通分解
    int k = scc();
    for(std::set<int>::iterator it = remain.begin(); it != remain.end(); it++) {
        if(  cmp[cur] > cmp[*it] || cmp[*it] > cmp[dst] ) {
            return false;
        }
        if(total_cost + dist[cur][*it] + dist[*it][dst] > optimal_cost)
            return false;
    }

    for(int i = 0; i < G[cur].size(); i++) {
        Edge &e = G[cur][i];
        bool ok = true;
        if(!visit[e.to]) {
            for(std::set<int>::iterator it = remain.begin(); it != remain.end(); it++) {
                if( cmp[cur] > cmp[*it] ) {
                    ok = false;
                    break;
                }
            }
            //拓扑序满足
            //采纳该节点
            if(ok) {
                visit[e.to] = 1;
                if(isMust[e.to]) {
                    remain.erase(e.to);
                    already.insert(e.to);
                }
                v_path.push_back(e.to);
                e_path.push_back(e.id);
                total_cost += e.cost;

                dfs_search_route(e.to);

                visit[e.to] = 0;
                if(isMust[e.to]) {
                    already.erase(e.to);
                    remain.insert(e.to);
                }
                v_path.pop_back();
                e_path.pop_back();
                total_cost -= e.cost;
            }//if(ok)

        }// if(!visit[e.to])
    }// for(int i = 0; i < G[cur].size(); i++)

    return false;
}


void SCC::search_route()
{
    start = clock();
    memset(visit, 0, sizeof(visit));

    visit[src] = 0;
    v_path.push_back(src);
    dfs_search_route(src);

    if(optimal_cost != 0) {
        for(std::vector<int>::iterator it = optimal_e_path.begin(); it != optimal_e_path.end(); it++)
            record_result(*it);
    }

    //freopen("output.txt", "w", stdout);

    printf("src=%d,dst=%d\n", src, dst);
    for(int i = 0; i < optimal_v_path.size(); i++)
        printf("%d->", optimal_v_path[i]);
    printf("Finish!\n");
    printf("optimal cost=%d\n", optimal_cost);

}


void floyd()
{
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++) {
            dist[i][j] = (i == j ? 0 : INF);
        }
    for(int v = 0; v < V; v++)
        for(int i = 0; i < G[v].size(); i++) {
            Edge &e = G[v][i];
            dist[v][e.to] = e.cost;
        }

    /* floyd */
    for(int k = 0; k < V; k++)
        for(int i = 0; i < V; i++)
            for(int j = 0; j < V; j++)
            {
                int temp = dist[i][k] + dist[k][j];
                if(temp < dist[i][j])
                    dist[i][j] = temp;
            }
}

bool Brute_Force::dfs(int cur)
{
    if(cur == dst) {
        for(std::set<int>::iterator it = neccesity.begin(); it != neccesity.end(); it++)
            if(!visit[*it])
                return false;

        if(optimal_cost >= INF) {
            optimal_cost = total_cost;
            optimal_e_path.assign(e_path.begin(), e_path.end());
            optimal_v_path.assign(v_path.begin(), v_path.end());
        }
        else if (total_cost < optimal_cost) {
            optimal_cost = total_cost;
            optimal_e_path.assign(e_path.begin(), e_path.end());
            optimal_v_path.assign(v_path.begin(), v_path.end());
        }
        return true;
    }

    //超时强制退出
    stop = clock();
    if((double)(stop - start)/CLOCKS_PER_SEC > 8.5)
        return false;

    for(int i = 0; i < G[cur].size(); i++) {
        Edge &e = G[cur][i];
        if(!visit[e.to]) {
            visit[e.to] = 1;
            v_path.push_back(e.to);
            e_path.push_back(e.id);
            total_cost += e.cost;

            dfs(e.to);

            visit[e.to] = 0;
            v_path.pop_back();
            e_path.pop_back();
            total_cost -= e.cost;
        }
    }

    return false;
}

void Brute_Force::search_route()
{
    start = clock();
    memset(visit, 0, sizeof(visit));

    visit[src] = 1;
    v_path.push_back(src);
    dfs(src);

    if(optimal_cost != 0) {
        for(std::vector<int>::iterator it = optimal_e_path.begin(); it != optimal_e_path.end(); it++)
            record_result(*it);
    }

    //freopen("output.txt", "w", stdout);

    printf("src=%d,dst=%d\n", src, dst);
    for(int i = 0; i < optimal_v_path.size(); i++)
        printf("%d->", optimal_v_path[i]);
    printf("Finish!\n");
    printf("optimal cost=%d\n", optimal_cost);
}

void Heuristic::bfs()
{
    start = clock();
    std::queue<int> Q;
    Edge pre[MAX_V]; //回溯使用的反向边记录数组
    std::vector<int> temp; //暂存搜索到的必经点
    std::stack<int> buf; //回溯顺序输出的辅助存储stack
    std::vector<int> subSeq; //局部优化子序列
    int beginNode = src;
    int new_beginNode = src;
    visit[src] = true;
    visit[dst] = true;
    bool done = false;
    Q.push(beginNode);
    while(!Q.empty()) {
        int cur = Q.front();
        Q.pop();
        for(int i = 0; i < G[cur].size(); i++) {
            Edge &e = G[cur][i];
            if(!bfsVisited[e.to] && !visit[e.to]) {
                bfsVisited[e.to] = true;
                Q.push(e.to);
                pre[e.to] = (Edge){e.id, cur, e.cost}; //记录前驱id,到达前驱的边id
                if(isMust[e.to])
                    temp.push_back(e.to);
            }
        }
        //bfs找到必经点
        if(temp.size() > 0) {
            //如果遇到多个必经点,随机选择一个
            cur = temp.at(random(0, temp.size()-1) );
            new_beginNode = cur;
            remain.erase(cur);
            already.insert(cur);
            //以该必经点为新的扩展起点
            //路径回溯
            while(cur != beginNode) {
                visit[cur] = true;
                cur = pre[cur].to;
            }
            visit[cur] = true;

            //清空暂存
            temp.clear();
            //清空队列
            while(!Q.empty()) Q.pop();
            memset(bfsVisited, 0, sizeof(bfsVisited));

            int already_num = already.size();
            int neccesity_num = neccesity.size();

            //所有必经点已找到
            if(already.size() == neccesity.size()) {
                done = true;
                break;
            }
            //否则继续以最新的必经点扩展下去
            beginNode = new_beginNode;
            Q.push(beginNode);
        }
    }

    visit[dst] = false;
    //去除已访问的点，在剩余图中求最后一个必经点buf到终点的最短路
    int d = Dijkstra(new_beginNode, dst, G, pre);

    //从终点回溯
    int cur = dst;
    while(cur != src) {
        visit[cur] = true;
        buf.push(pre[cur].id); //回溯一条边，并将其压栈
        total_cost += pre[cur].cost;
        cur = pre[cur].to; //找前驱
    }
    while(!buf.empty()) {
        int e = buf.top();
        buf.pop();
        e_path.push_back(e);
    }

    while(!buf.empty()) buf.pop();
    cur = dst;
    while(cur != src) {
        buf.push(cur);//回溯前驱，并将其压栈
        cur = pre[cur].to;
    }
    buf.push(cur);
    while(!buf.empty()) {
        int node = buf.top();
        buf.pop();
        v_path.push_back(node);
    }

    //freopen("output.txt", "w", stdout);
    printf("----------result----------\n");
    if(!done) {
        printf("neccesity node miss!\n");
        for(std::set<int>::iterator it = remain.begin(); it != remain.end(); it++)
            printf("%d", *it);
        printf("\n");
    }
    printf("route:\n");
    for(int i = 0; i < v_path.size(); i++) {
        printf("%d->", v_path[i]);
    }
    printf("Finish!\n");
    printf("cost = %d\n", total_cost);
    printf("----------result----------\n");

    for(int i = 0; i < e_path.size(); i++)
        record_result(e_path[i]);

    //结果调优
    int new_cost, cost;
    for(;;) {
    //超时强制退出
    stop = clock();
    if((double)(stop - start)/CLOCKS_PER_SEC > 8.5)
        break;
    int head, tail;
    head = random(0, v_path.size()-2);
    //printf("head = %d\n", head);
    tail = head + 1;
    while(!isMust[v_path[tail]]) tail++;
    //对[head, tail]段进行优化
    int cost = 0;
    int v = v_path[tail];
    while(v != v_path[head]){
        cost += pre[v].cost;
        visit[v] = false;
        v = pre[v].to;
    }
    visit[v] = false;
    new_cost = sub_sequence_optimize(v_path[head], v_path[tail], cost, G, pre);
    if(new_cost < cost) {
        printf("optimizing....\n");
        //对e_path[head, tail-1]进行置换il
        for(int i = tail; i > head; i--) {
            e_path[i - 1] = pre[v_path[i]].id;
        }
        total_cost -= cost - new_cost;
    }
    }
    printf("new cost = %d\n", total_cost);

}

int Heuristic::sub_sequence_optimize(int src, int dst, int cost, std::vector<Edge> G[MAX_V], Edge pre[MAX_V])
{
    int D[MAX_V];
    for(int i=0; i < V; i++)
        D[i] = INF;
    D[src] = 0;
    D[dst] = cost;

    std::priority_queue<pair_i_i, std::vector<pair_i_i>, std::greater<pair_i_i> > Q;

    Q.push(std::make_pair(D[src], src));
    while(!Q.empty()) {
        pair_i_i x = Q.top();
        Q.pop();
        int cur = x.second;
        if(x.first != D[cur]) continue;
        if( cur == dst)
            break;

        for(int i = 0; i < G[cur].size(); i++) {
            Edge &e = G[cur][i];
            //松弛操作
            if(!visit[e.to] && D[cur] + e.cost < D[e.to]) {
                D[e.to] = D[cur] + e.cost;
                Q.push(std::make_pair(D[e.to], e.to));
                pre[e.to] = (Edge){e.id, cur, e.cost};
            }
        }
    }
    return D[dst];

}
int random(int x, int y)
{
    srand((unsigned) time(NULL));
    return x + rand()%(y - x + 1);
}

int Dijkstra(int src, int dst, std::vector<Edge> G[MAX_V], Edge pre[MAX_V])
{
    int D[MAX_V];
    for(int i=0; i < V; i++)
        D[i] = INF;
    D[src] = 0;

    std::priority_queue<pair_i_i, std::vector<pair_i_i>, std::greater<pair_i_i> > Q;

    Q.push(std::make_pair(D[src], src));
    while(!Q.empty()) {
        pair_i_i x = Q.top();
        Q.pop();
        int cur = x.second;
        if(x.first != D[cur]) continue;
        if( cur == dst)
            break;

        for(int i = 0; i < G[cur].size(); i++) {
            Edge &e = G[cur][i];
            //松弛操作
            if(!visit[e.to] && D[cur] + e.cost < D[e.to]) {
                D[e.to] = D[cur] + e.cost;
                Q.push(std::make_pair(D[e.to], e.to));
                pre[e.to] = (Edge){e.id, cur, e.cost};
            }
        }

    }
    return D[dst];

}












