#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

int dp_search(Graph &G, int dp[][20], int route[][20], int S, int v, int demand);

void dp_search_route(Graph &G);

#endif // ALGO_H_INCLUDED
