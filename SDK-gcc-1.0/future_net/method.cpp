#include "lib/lib_record.h"
#include "method.h"
#include <stdio.h>
#include <stdlib.h>
#include <string> //string
#include <set>
#include <vector>
#include <string.h> //memset

#define MAX_V 600

//边定义
typedef struct Edge
{
    int id;
    int to;
    int cost;
}Edge;

std::vector<Edge> G[MAX_V]; //图的邻接表
std::vector<Edge> rG[MAX_V]; //反向边邻接表

std::vector<int> PostOrder; //scc第一次dfs过程中的后序存放数组
bool used[MAX_V]; //scc在dfs过程中的访问标记数组
int cmp[MAX_V]; //拓扑序
int depth;
int cnt = 0;

int src, dst; //起点，终点
int V = 0, E = 0; //顶点数，边数
std::set<int> neccesity; //必经点集合
std::set<int> already; //已访问必经点
std::set<int> remain; //待访问必经点
bool isMust[MAX_V]; //是否为必经点

std::vector<int> v_path; //节点表示的路径
std::vector<int> e_path; //边表示的路径
bool visit[MAX_V]; //已访问节点标记


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

bool SCC::dfs_search_route(int cur, int edge)
{
    if(cur == dst) {
        for(std::set<int>::iterator it = neccesity.begin(); it != neccesity.end(); it++)
            if(!visit[*it])
                return false;
        v_path.push_back(cur);
        e_path.push_back(edge);
        return true;
    }

    int k = scc();
    for(std::set<int>::iterator it = remain.begin(); it != remain.end(); it++) {
        if(  cmp[cur] > cmp[*it] || cmp[*it] > cmp[dst] ) { //belongs[cur] > belongs[*it] ||
            cnt ++;
            return false;
        }
    }

    visit[cur] = 1;
    v_path.push_back(cur);
    if(cur != src)
        e_path.push_back(edge);
    if(isMust[cur]) {
        remain.erase(cur);
        already.insert(cur);
    }

    for(int i = 0; i < G[cur].size(); i++) {
        Edge &e = G[cur][i];
        if( !visit[e.to] )
            if( dfs_search_route(e.to, e.id) )
                return true;
    }

    visit[cur] = 0;
    if(isMust[cur]) {
        already.erase(cur);
        remain.insert(cur);
    }
    v_path.pop_back();
    e_path.pop_back();

    return false;
}

void SCC::search_route()
{
    memset(visit, 0, sizeof(visit));
    depth = 5;
    if(dfs_search_route(src)) {
        for(std::vector<int>::iterator it = e_path.begin(); it != e_path.end(); it++)
            record_result(*it);
    }

    printf("src=%d,dst=%d\n", src, dst);
    for(int i = 0; i < v_path.size(); i++)
        printf("%d->", v_path[i]);
    printf("Finish!\n");
    printf("cnt=%d\n", cnt);


}















