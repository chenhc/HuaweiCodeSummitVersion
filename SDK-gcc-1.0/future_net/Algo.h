#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED


void Floyd(Graph &G, DistMatrix dist);

int dfs(Graph &G, int cur, int dst, Route &path);

void dfs_search_route(Graph &G);

int min_cost_max_flow(int n,int mat[][nMAX],int cost[][nMAX], int source, int sink, int flow[][nMAX],int& netcost);

void min_cost_max_flow_find_route(Graph &G);

#endif // ALGO_H_INCLUDED
