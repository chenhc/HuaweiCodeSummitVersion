#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

int dp_search(Graph &G, int S, int v, int demand);

void dp_search_route(Graph &G);

void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);

void cluster(Graph &G, DistMatrix dist);

int partial_dfs(Graph &G, int cur, int dst, vector<int> &route, int visit[nMAX]);

int specified_partial_connect(Graph &G, Route &route, DistMatrix dist);

#endif // ALGO_H_INCLUDED
