#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

int dp_search(Graph &G, int S, int v, int demand);

void dp_search_route(Graph &G);

void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path, int range = INF);

void dfs_search_route(Graph &G);

int dfs_2(Graph &G, int cur, int dst, Route &route, int range = INF);

void dfs_search_route_2(Graph &G);

int greedy_dfs(Graph &G, DistMatrix dist, int cur, int dst, Route &route, int range = INF);

int greedy_dfs_for_dst(Graph &G, DistMatrix dist, int cur, int dst, Route &route, int range = INF);

#endif // ALGO_H_INCLUDED
