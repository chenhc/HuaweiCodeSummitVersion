#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

int dp_search(Graph &G, int S, int v, int demand);

void dp_search_route(Graph &G);

void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);

void cluster(Graph &G, DistMatrix dist, Component components[], int cluster_num);

int partial_dfs(Graph &G, int cur, int dst, vector<int> &route, int visit[nMAX], int radius);

int partial_connect(Graph &G, Component &component, int visit[nMAX]);

int components_connect(Graph &G, Component components[], int cluster_num);

int fully_connect(Graph &G, Component components[], int cluster_num);

int components_interconnect(Graph &G, DistMatrix dist, Component components[], int cluster_num, int visit[nMAX]);

void divide_search_route(Graph &G);


#endif // ALGO_H_INCLUDED
