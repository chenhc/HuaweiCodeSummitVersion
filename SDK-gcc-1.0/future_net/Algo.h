#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED


void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);

int bi_bfs(Graph &G, int src, int dst, Route &route);

#endif // ALGO_H_INCLUDED
