#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <stdlib.h>
#include <string> //string
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <queue>
#include <string.h> //memset
#include <time.h>


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
    State _begin = State(src);
    std::priority_queue<State, std::vector<State>, std::less<State> > OPEN;
    std::map<int, State> CLOSED;

    bool success = false;
    OPEN.push(_begin);
    while(!OPEN.empty() && !success) {
        //从open移除
        State father = OPEN.top();
        OPEN.pop();
        //bfs扩展遇到终点了，并且必经点访问完毕！
        if(father.cur == dst && father.already == neccesity.size()) {
            success = true;
            break;
        }
        //加入close
        int cur = father.cur;
        CLOSED.insert(std::make_pair(cur, father));
        //状态扩展
        for(int i = 0; i < G[cur].size(); i++) {
            Edge &e = G[cur][i];
            State child(e.to);
            child.steps = father.steps++;
            child.already = father.already;
            child.pre = cur;
            child.rEdge = e.id;
            if(isMust[e.to])
                child.already ++;

            //如果在close中
            if(CLOSED.find(e.to) != CLOSED.end()) {
                //但是新节点的必经点指标更优
                if(child.already > CLOSED[e.to].already) {
                    CLOSED.erase(e.to);
                }
            }

            //加入open,即使队列有相同元素，优先级覆盖
            OPEN.push(child);
        }
    }

}












