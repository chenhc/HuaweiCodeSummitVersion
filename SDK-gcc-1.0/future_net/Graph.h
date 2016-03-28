#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <vector>
#include <set>

#define nMAX 300
#define lMAX 1000
#define INF 99999

using namespace std;

typedef int DistMatrix[nMAX][nMAX];

class Link
{
public:
    int linkID;
    int src;
    int dst;
    int cost;

    Link(): linkID(-1), src(-1), dst(-1), cost(INF) {}

    Link(int _linkid, int _src, int _dst, int _cost)
    : linkID(_linkid), src(_src), dst(_dst), cost(_cost) {}

    bool operator < (const Link &e) const
    {
        return cost < e.cost;
    }
};

class Graph
{
public:
    int src;
    int dst;
    int lNum;
    int nNum;
    int specified_num;

    Link Edge[lMAX];
    int Specified[60]; /* MAX 60 nodes must be visited */
    int must[nMAX]; /*if node is specified, _must[node]=1*/

    int first[nMAX];
    int next[lMAX];
    int pre_first[nMAX];
    int pre_next[lMAX];

    Graph(char *topo[],  int edge_num, char *demand);
};

class Route
{
public:
    int cost;
    vector<int> path; /* record the edges along the route*/
    /* record the id of the specified nodes which already visited*/
    int visit[nMAX]; /* record if the node has been visited */

    Route();
};


#endif // GRAPH_H_INCLUDED

