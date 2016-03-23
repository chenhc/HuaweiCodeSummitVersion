#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <vector>
#include <set>

#define nMAX 600
#define lMAX 5000
#define INF 999999

using namespace std;

typedef int DistMatrix[nMAX][nMAX];

class Link
{
public:
    int _linkID;
    int _src;
    int _dst;
    int _cost;

    Link(): _linkID(-1), _src(-1), _dst(-1), _cost(INF) {}

    Link(int linkid, int src, int dst, int cost)
    : _linkID(linkid), _src(src), _dst(dst), _cost(cost) {}

    bool operator < (const Link &e) const
    {
        return _cost < e._cost;
    }
};

class Component
{
public:
    int _head;
    int _tail;
    vector<int> _elems;
    vector<int> _path;

    Component() : _head(-1), _tail(-1) {}
    Component(int src, int dst) : _head(src), _tail(dst) {}
};

class Graph
{
public:
    int _src;
    int _dst;
    int _lNum;
    int _nNum;
    int specified_num;

    Link _Edge[lMAX];
    int _Specified[60]; /* MAX 60 nodes must be visited */
    int _must[nMAX]; /*if node is specified, _must[node]=1*/

    int _first[nMAX];
    int _next[lMAX];
    int _pre_first[nMAX];
    int _pre_next[lMAX];

    Graph(char *topo[],  int edge_num, char *demand);
};

class Route
{
public:
    int _cost;
    vector<int> _path; /* record the edges along the route*/
    /* record the id of the specified nodes which already visited*/
    int _visit[nMAX]; /* record if the node has been visited */

    Route();
};

#endif // GRAPH_H_INCLUDED

