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


std::vector<Edge> G[MAX_V]; //ͼ���ڽӱ�
std::vector<Edge> rG[MAX_V]; //������ڽӱ�

std::vector<int> PostOrder; //scc��һ��dfs�����еĺ���������
bool used[MAX_V]; //scc��dfs�����еķ��ʱ������
int cmp[MAX_V]; //������

int src, dst; //��㣬�յ�
int V = 0, E = 0; //������������
std::set<int> neccesity; //�ؾ��㼯��
std::set<int> already; //�ѷ��ʱؾ���
std::set<int> remain; //�����ʱؾ���
bool isMust[MAX_V]; //�Ƿ�Ϊ�ؾ���

std::vector<int> v_path; //�ڵ��ʾ��·��
std::vector<int> e_path; //�߱�ʾ��·��
bool visit[MAX_V]; //�ѷ��ʽڵ���
std::vector<int> optimal_e_path;
std::vector<int> optimal_v_path;
int total_cost = 0;
int optimal_cost = INF;

int start, stop;

DistMatrix dist; //�������

//��Ҫ����ʵ��
//��ӱ�
void add_edge(int id, int from, int to, int cost)
{
    G[from].push_back((Edge){id, to, cost});
    rG[to].push_back((Edge){id, from, cost});
}

//��ȡ��������
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

//scc��dfs����
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
    //��ǰ�ߵ����յ�
    if(cur == dst) {
        for(std::set<int>::iterator it = neccesity.begin(); it != neccesity.end(); it++)
            if(!visit[*it])
                return false;

        //��ǰ�ߵ����յ㣬�Ҿ������бؾ���
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

    //��ʱǿ���˳�
    stop = clock();
    if((double)(stop - start)/CLOCKS_PER_SEC > 8.5)
        return false;

    //ǿ��ͨ�ֽ�
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
            //����������
            //���ɸýڵ�
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

    //��ʱǿ���˳�
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
        //��open�Ƴ�
        State father = OPEN.top();
        OPEN.pop();
        //bfs��չ�����յ��ˣ����ұؾ��������ϣ�
        if(father.cur == dst && father.already == neccesity.size()) {
            success = true;
            break;
        }
        //����close
        int cur = father.cur;
        CLOSED.insert(std::make_pair(cur, father));
        //״̬��չ
        for(int i = 0; i < G[cur].size(); i++) {
            Edge &e = G[cur][i];
            State child(e.to);
            child.steps = father.steps++;
            child.already = father.already;
            child.pre = cur;
            child.rEdge = e.id;
            if(isMust[e.to])
                child.already ++;

            //�����close��
            if(CLOSED.find(e.to) != CLOSED.end()) {
                //�����½ڵ�ıؾ���ָ�����
                if(child.already > CLOSED[e.to].already) {
                    CLOSED.erase(e.to);
                }
            }

            //����open,��ʹ��������ͬԪ�أ����ȼ�����
            OPEN.push(child);
        }
    }

}












