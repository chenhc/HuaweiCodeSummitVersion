#ifndef __ROUTE_H__
#define __ROUTE_H__

#define nMAX 600+50
#define lMAX 5000
#define INF 99999

typedef struct
{
    int linkID;
    int src;
    int dst;
    int cost;
}Link;

void search_route(char *graph[5000], int edge_num, char *condition);

int recursion(int S, int v, const int demand);

int strtoi(const char * str, int len);

#endif
