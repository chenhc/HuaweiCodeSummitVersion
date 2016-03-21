#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

int dp_search(Graph &G, int S, int v, int demand);

void dp_search_route(Graph &G);

void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);


#endif // ALGO_H_INCLUDED
