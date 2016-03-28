#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED


void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);

int dfs(Graph &G, int center, int radiu, bool visit[lMAX]);

int greedy_dfs(Graph &G, int cur, int dst, Route &route);

void Dijkstra(Graph &G, int src, int D[nMAX], int path[nMAX]);

int greedy_search_route(Graph &G);

#endif // ALGO_H_INCLUDED
