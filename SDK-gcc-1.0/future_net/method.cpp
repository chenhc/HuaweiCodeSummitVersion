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

bool bfsVisited[MAX_V];

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
    start = clock();
    std::queue<int> Q;
    Edge pre[MAX_V]; //����ʹ�õķ���߼�¼����
    std::vector<int> temp; //�ݴ��������ıؾ���
    std::stack<int> buf; //����˳������ĸ����洢stack
    std::vector<int> subSeq; //�ֲ��Ż�������
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
                pre[e.to] = (Edge){e.id, cur, e.cost}; //��¼ǰ��id,����ǰ���ı�id
                if(isMust[e.to])
                    temp.push_back(e.to);
            }
        }
        //bfs�ҵ��ؾ���
        if(temp.size() > 0) {
            //�����������ؾ���,���ѡ��һ��
            cur = temp.at(random(0, temp.size()-1) );
            new_beginNode = cur;
            remain.erase(cur);
            already.insert(cur);
            //�Ըñؾ���Ϊ�µ���չ���
            //·������
            while(cur != beginNode) {
                visit[cur] = true;
                cur = pre[cur].to;
            }
            visit[cur] = true;

            //����ݴ�
            temp.clear();
            //��ն���
            while(!Q.empty()) Q.pop();
            memset(bfsVisited, 0, sizeof(bfsVisited));

            int already_num = already.size();
            int neccesity_num = neccesity.size();

            //���бؾ������ҵ�
            if(already.size() == neccesity.size()) {
                done = true;
                break;
            }
            //������������µıؾ�����չ��ȥ
            beginNode = new_beginNode;
            Q.push(beginNode);
        }
    }

    visit[dst] = false;
    //ȥ���ѷ��ʵĵ㣬��ʣ��ͼ�������һ���ؾ���buf���յ�����·
    int d = Dijkstra(new_beginNode, dst, G, pre);

    //���յ����
    int cur = dst;
    while(cur != src) {
        visit[cur] = true;
        buf.push(pre[cur].id); //����һ���ߣ�������ѹջ
        total_cost += pre[cur].cost;
        cur = pre[cur].to; //��ǰ��
    }
    while(!buf.empty()) {
        int e = buf.top();
        buf.pop();
        e_path.push_back(e);
    }

    while(!buf.empty()) buf.pop();
    cur = dst;
    while(cur != src) {
        buf.push(cur);//����ǰ����������ѹջ
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

    //�������
    int new_cost, cost;
    for(;;) {
    //��ʱǿ���˳�
    stop = clock();
    if((double)(stop - start)/CLOCKS_PER_SEC > 8.5)
        break;
    int head, tail;
    head = random(0, v_path.size()-2);
    //printf("head = %d\n", head);
    tail = head + 1;
    while(!isMust[v_path[tail]]) tail++;
    //��[head, tail]�ν����Ż�
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
        //��e_path[head, tail-1]�����û�il
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
            //�ɳڲ���
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
            //�ɳڲ���
            if(!visit[e.to] && D[cur] + e.cost < D[e.to]) {
                D[e.to] = D[cur] + e.cost;
                Q.push(std::make_pair(D[e.to], e.to));
                pre[e.to] = (Edge){e.id, cur, e.cost};
            }
        }

    }
    return D[dst];

}












