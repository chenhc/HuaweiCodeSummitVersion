#include "route.h"
#include "lib/lib_record.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

extern Link Edge[lMAX];
extern int first[nMAX], next[lMAX], visit[nMAX], must[nMAX];
extern int Src, Dst, node_num, must_num;

int route[1<<20][20];
int dp[1<<20][20];

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    /*
    unsigned short result[] = {2, 6, 3};//示例中的一个解

    for (int i = 0; i < 3; i++)
        record_result(result[i]);
    */
    int linkID, srcID, destID, cost, n_cnt = 0;
    memset(first, -1, sizeof(first));

    for(int i = 0; i <edge_num; i++)
    {
       sscanf(topo[i], "%d,%d,%d,%d", &linkID, &srcID, &destID, &cost);
       Edge[i].linkID = linkID;
       Edge[i].src = srcID;
       Edge[i].dst = destID;
       Edge[i].cost = cost;
       int temp = srcID>destID? srcID : destID;
       n_cnt = n_cnt>temp ? n_cnt : temp;
       next[i] = first[srcID];
       first[srcID] = i;
    }
    node_num = n_cnt + 1;
    if(node_num >20)
        return;

    char buf[100];
    sscanf(demand, "%d,%d,%s" , &Src, &Dst, buf);
    int cnt = 0;
    int len = strlen(buf);
    char temp[5];
    int i = 0, j = 0;
    while(i<len)
    {
        if( isdigit(buf[i]) )
            temp[j++] = buf[i];

        if( i==len-1)
        {
            must[cnt] = strtoi(temp, j);
            j = 0;
            cnt ++;
        }
        else if(buf[i]=='|')
        {
            must[cnt] = strtoi(temp, j);
            j = 0;
            cnt ++;
        }
        i++;
    }
    must_num = cnt;

    memset(dp, -1, sizeof(dp));
    int _demand = 0;
    for(int i=0; i<must_num; i++)
    {
        _demand = _demand | (1<<must[i]);
    }
    _demand = _demand | (1<<Dst);

    int result = recursion(1<<Src, Src, _demand);
    if(result >= INF)
        return;

    int u = Src, S = 1<<Src, e = -1;
    while(u != Dst)
    {
        e = route[S][u];
        u = Edge[e].dst;
        S = S | (1<<u);
        record_result(Edge[e].linkID);
    }

}

int recursion(int S, int v, const int demand)
{
    /*¼ÇÒä»¯ËÑË÷*/
    if(dp[S][v] >= 0)
        return dp[S][v];
    if((S & demand) == demand && v == Dst)
        return dp[S][v] = 0;

    int res = INF;
    for(int e = first[v]; e != -1; e = next[e])
    {
        int u = Edge[e].dst;
        if(!(S>>u & 1))
        {
            int temp = Edge[e].cost + recursion(S|1<<u, u, demand);
            if(temp < res )
            {
                res = temp;
                route[S][v] = e;
            }
        }
    }

    return dp[S][v] = res;
}

int strtoi(const char *str, int len)
{
    int k,result=0;
    for(k=0; k<len; k++)
    {
        result += (str[k]-'0') * (int)pow(10, len-1-k);
    }
    return result;
}
