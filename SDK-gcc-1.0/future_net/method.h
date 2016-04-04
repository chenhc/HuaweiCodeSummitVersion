#ifndef METHOD_H_INCLUDED
#define METHOD_H_INCLUDED

#define MAX_V 600
#define INF 999999

typedef struct Edge
{
    int id;
    int to;
    int cost;
}Edge;

typedef int DistMatrix[MAX_V][MAX_V];

void add_edge(int id, int from, int to, int cost);

void init_Graph(char *topo[5000], int edge_num, char *demand);

void floyd();

namespace SCC {
    void dfs(int v);
    void rdfs(int v, int k);
    int scc();
    bool dfs_search_route(int cur);
    void search_route();
}

namespace Brute_Force {
    bool dfs(int cur);
    void search_route();
}

namespace Heuristic {
    typedef struct State
    {
        int cur;//��ǰ�ڵ�
        int already;//�Ѿ����ؾ���ĸ���
        int steps;
        int pre; //ǰ���ڵ�
        int rEdge; //�����·

        bool operator < (const State &s) const //�ؾ�����������
        {
            if(already < s.already) return true;
            else if(already == s.already) {
                if(steps > s.steps)
                    return true;
            }
            return false;
        }
        State(){}
        State(int _id, int _already = 0, int _steps = 0, int _pre = -1, int _rEdge = -1):
                cur(_id), already(_already), steps(_steps), pre(_pre), rEdge(_rEdge)
        {}
    }State;

    void bfs();
}

int random(int x, int y)
{
    return x + rand()%(y - x + 1);
}
#endif // METHOD_H_INCLUDED
