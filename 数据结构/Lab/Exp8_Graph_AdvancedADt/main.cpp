#include <cstdio>
#include <cstdlib>
#include <queue>
#include <stack>
/**基于邻接表实现图的DFS,BFS,Dijkstra,求连通片个数* **/
#define DEBUG 0
#define MAX_VERTEX_NUM 20
#define P73VER_NUM 8
#define INFINITE 9999
typedef enum{DG,DN,UDG,UDN} GraphKind; //图的类型,UDG无向无权图
int DeletedVexNum=0;

//图的邻接表表示 静态内存结构
//Arc弧结构体
typedef struct ArcNode {
    int adjvex;             //该弧指向的顶点位置
    int weight;             //弧的权值
    struct ArcNode *nextarc;    //指向下一条弧指针
}ArcNode;
//Vertex结点结构体
typedef struct VNode {
    int id;             //结点id号
    int value;          //结点的值
    ArcNode *firstarc; //该结点的第一条弧
}VNode,AdjList[MAX_VERTEX_NUM];
//图结构体，邻接表实现
typedef struct {
    AdjList vertices;
    int vexnum,arcnum;
    GraphKind kind;
}ALGraph;
///————————————————————————————————————————————————————————————————————————
//打印邻接表的信息
void printG(ALGraph *g){
    printf("图的基本信息：顶点数(%u)-边数(%u)-%s%s\n",g->vexnum,g->arcnum,\
           (g->kind==0||g->kind==2)?"无权":"带权",g->kind>1?"无向图":"有向图");
    for(int i=0;i< g->vexnum + DeletedVexNum; ++i){
        if(g->vertices[i].id!=-1) {
            printf("ID(%u)-值(%d)-弧表 (指向结点id|弧的权值weight)：", g->vertices[i].id, g->vertices[i].value);
            for (ArcNode *p = g->vertices[i].firstarc; p != nullptr; p = p->nextarc) {
                printf("-> (%d|%d) ", p->adjvex, p->weight);
            }
            printf("\n");
        }
    }
}
///————————————————————————————————————————————————————————————————————————
//给一个结点的值，返回结点ID
int LocateVexbyValue(ALGraph G , int v){
    for (int k=0; k<G.vexnum+DeletedVexNum; k++)
        if (G.vertices[k].value == v) {
#if DEBUG
            printf("找到value为%d的结点id为：%d\n", v, G.vertices[k].id);
#endif
            return G.vertices[k].id;
        }
    printf("未找到该结点！\n");
    return -1;
}
//给一个结点的id，返回结点的值
int LocateVexbyID(ALGraph G , int id){
    for (int k=0; k<G.vexnum+DeletedVexNum; k++)
        if (G.vertices[k].id == id) {
#if DEBUG
            printf("找到id为%d的结点value为：%d\n", id, G.vertices[k].value);
#endif
            return G.vertices[k].value;
        }
    printf("未找到该结点！\n");
    return -1;
}
//获取v1->v2弧的权值
int GetArcWeight(ALGraph G,int v1,int v2){
    ArcNode* tmp;
    tmp=G.vertices[v1].firstarc;
    while (tmp&&tmp->adjvex!=v2)
        tmp=tmp->nextarc;
    if (tmp&&tmp->adjvex==v2)
        return tmp->weight;
    return INFINITE;
}
///————————————————————————————————————————————————————————————————————————
//创建一个空图
ALGraph *Create_Graph(ALGraph *G){
/*    printf("请输入图的种类标志(0=DG;1=DN;2=UDG;3=UDN):\n");
    scanf("%d", &G->kind);*/
    G->kind=UDN;
    G->vexnum=0; //初始化顶点个数 return(G);
    G->arcnum=0;
    //初始化firstarc
    for (auto & vertice : G->vertices)
        vertice.firstarc = nullptr;
    switch (G->kind) {
        case DG:
            printf("创建成功，图的种类为DG有向无权图\n");
            break;
        case DN:
            printf("创建成功，图的种类为DN有向带权图\n");
            break;
        case UDG:
            printf("创建成功，图的种类为UDG无向无权图\n");
            break;
        case UDN:
            printf("创建成功，图的种类为UDN无向带权图\n");
            break;
        default:
            printf("输入错误，程序退出！\n");
            exit(-1);
    }
    return G;
}
///————————————————————————————————————————————————————————————————————————
//创建非空图
ALGraph *creat()
{
    auto *G=(ALGraph*)malloc(sizeof(ALGraph));
    printf("请输入顶点数目：");
    scanf("%d", &(G->vexnum));
    printf("请输入弧的数目：");
    scanf("%d", &(G->arcnum));

    int i,k;
    int vex;
    int v1, v2,info;
    printf("请输入顶点value：\n");
    for (i = 0; i < G->vexnum; i++)
    {
        scanf("%d", &vex);
        G->vertices[i].value = vex;
        G->vertices[i].firstarc = nullptr;
    }

    printf("请输入弧的信息：\n");
    for (k = 0; k < G->arcnum; k++)
    {
        scanf("%d%d%d", &v1, &v2,&info);
        int a = LocateVexbyValue(*G, v1);
        int b = LocateVexbyValue(*G, v2);
        auto *p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = b;
        p->weight= info;
        p->nextarc = G->vertices[a].firstarc;
        G->vertices[a].firstarc = p;
    }
    return G;
}
///————————————————————————————————————————————————————————————————————————
//在顶点数组AdjList末尾增加若干个值为value的数据元素，返回当前结点个数
int AddVertex(ALGraph *G){
    int addnum;
/*    printf("需要添加几个结点？\n");
    scanf("%d",&addnum);*/
    addnum=P73VER_NUM;
    //检查结点个数
    //若之前有删除过的结点
    if(addnum<=DeletedVexNum){
        printf("请依次输入这%d个结点的value\n", addnum);
        while(addnum>0){
            int v;
            scanf("%d", &v);
            //寻找id=-1的位置,将结点添加在此处
            for(int i=0;i< G->vexnum + DeletedVexNum; ++i)
                if(G->vertices[i].id==-1){
                    G->vertices[i].id = i;
                    G->vertices[i].value = v;
                    G->vertices[i].firstarc = nullptr;
                    G->vexnum++;
                    break;
                }
            addnum--;
            DeletedVexNum--;
        }
    }
        //若之前删除过的结点数小于要添加的结点数
    else {
        if (G->vexnum + DeletedVexNum + addnum >= MAX_VERTEX_NUM) {
            printf("结点数量溢出，不允许!\n");
            return G->vexnum;
        }
        //依次输入结点value，增加结点
        printf("请依次输入这%d个结点的value\n", addnum);
        for (int i = 1; i <= addnum; i++) {
/*            int v;
            scanf("%d", &v);*/
            int v=1;
            int k = G->vexnum + DeletedVexNum;    //当前结点位置
            //在顶点数组AdjList末尾增加一个数据元素
            G->vertices[k].id = k;
            G->vertices[k].value = v;
            G->vertices[k].firstarc = nullptr;
            G->vexnum++;
        }
    }
    return G->vexnum;
}
///————————————————————————————————————————————————————————————————————————
//头插法向图中增加一条弧，输入起点id2，终点id2，权值weight
int AddArc(ALGraph *G,int id1,int id2,int w){
    if( LocateVexbyID(*G,id1)==-1 || LocateVexbyID(*G,id2)==-1){
        printf("弧依附的顶点id不存在!\n");
        return G->arcnum;
    }
    auto *p=(ArcNode*)malloc(sizeof(ArcNode));   //id1指向id2的弧
    p->adjvex=id2;
    p->weight=w;
    p->nextarc=nullptr;
    auto *q=(ArcNode*)malloc(sizeof(ArcNode));   //id2指向id1的弧
    q->adjvex=id1;
    q->weight=w;
    q->nextarc=nullptr;
    //无向图用头插法插入到两个单链表中去
    if(G->kind==2||G->kind==3){
        p->nextarc=G->vertices[id1].firstarc;
        G->vertices[id1].firstarc=p;
        q->nextarc=G->vertices[id2].firstarc;
        G->vertices[id2].firstarc=q;
    }
        //有向图，正向邻接表用
    else{
        p->nextarc=G->vertices[id1].firstarc;
        G->vertices[id1].firstarc=p;
    }

    G->arcnum++;
    return G->arcnum;
}
///————————————————————————————————————————————————————————————————————————
//广度优先搜索，返回联通片的个数
int BFS(ALGraph *g,int goal) {
    int visited[P73VER_NUM] = {0};
    std::queue<int> Q;
    int component=0;
    printf("广度优先搜索路径：");
    for (int v = 0; v <= g->vexnum; v++) {
        if (!visited[v]) {
            component++;                            //计算联通片个数
            visited[v] = 1;
            printf("%d ", g->vertices[v].id);       //标记顶点并打印顶点id
            //判断goal函数
            if(goal==g->vertices[v].id) return component;
            Q.push(v);                              //入队列
            while (!Q.empty()) {                    //队列非空
                int u = Q.front();                  //u为对首元素
                Q.pop();                               //出队
                //遍访u的所有邻接点
                for (ArcNode *tmp = g->vertices[u].firstarc; tmp != nullptr; tmp = tmp->nextarc) {
                    int w=tmp->adjvex;
                    if (!visited[w]) {
                        visited[w] = 1;
                        printf("%d ", g->vertices[w].id);         //标记顶点并打印顶点id
                        //判断goal函数
                        if(goal==g->vertices[w].id) return component;
                        Q.push(w);                             //入队列
                    }
                }
            }
        }
    }
    if(goal==-1)
        printf("  没有满足goal函数的结点，搜索退化为遍历；该图的连通片个数为%d\n",component);
    return component;
}
//深度优先搜索，返回联通片的个数
int DFS(ALGraph *g,int goal) {
    int visited[P73VER_NUM] = {0};
    std::stack<int> S;
    int component=0;
    printf("深度优先搜索路径：");
    for (int v = 0; v <= g->vexnum; v++) {
        if (!visited[v]) {
            component++;                            //计算联通片个数
            S.push(v);                              //首结点入栈
            while (!S.empty()) {
                int u = S.top();
                S.pop();                             //出队
                //u没访问过就访问，并将u所有未被访问过的邻接点入栈
                if (!visited[u]) {
                    visited[u] = 1;
                    printf("%d ", g->vertices[u].id);       //标记顶点并打印顶点id
                    //判断goal函数
                    if(goal==g->vertices[u].id) return component;
                    //u所有未被访问过的邻接点入栈
                    for (ArcNode *tmp2 = g->vertices[u].firstarc; tmp2 != nullptr; tmp2 = tmp2->nextarc)
                        if (!visited[tmp2->adjvex])
                            S.push(tmp2->adjvex);
                }
            }
        }
    }
    if(goal==-1)
        printf("  没有满足goal函数的结点，搜索退化为遍历；该图的连通片个数为%d\n",component);
    return component;
}
///————————————————————————————————————————————————————————————————————————
//构建边
void ADDArcsforExp8(ALGraph *g){
    AddArc(g,0,1,1);
    AddArc(g,0,2,1);
    AddArc(g,0,4,1);
    AddArc(g,0,5,1);
    AddArc(g,1,2,1);
    AddArc(g,1,3,1);
    AddArc(g,3,2,1);
    AddArc(g,3,4,1);
    AddArc(g,3,5,1);
    AddArc(g,4,5,1);
    AddArc(g,6,7,1);
}
void ADDArcsforDijkstra(ALGraph *g){
    AddArc(g,0,1,1);
    AddArc(g,0,2,2);
    AddArc(g,0,4,4);
    AddArc(g,0,5,7);
    AddArc(g,0,6,8);
    AddArc(g,1,2,2);
    AddArc(g,1,3,3);
    AddArc(g,1,6,7);
    AddArc(g,2,3,1);
    AddArc(g,2,5,3);
    AddArc(g,3,5,3);
    AddArc(g,3,7,6);
    AddArc(g,4,5,3);
    AddArc(g,4,6,2);
    AddArc(g,4,7,6);
    AddArc(g,5,7,4);
    AddArc(g,6,7,4);
}
///————————————————————————————————————————————————————————————————————————
void DijkstraPath(ALGraph G,int v0){
    int dist[MAX_VERTEX_NUM];            //dist[i]:v0->vi的最短路径长度
    int set[MAX_VERTEX_NUM];             //set[i]:顶点是否已经加入路径
    int path[MAX_VERTEX_NUM];            //path[i]:保存v0->vi路径上vi的前继结点
    std::stack<int> S;
    int min,v;
    //数组初始化
    for(int i=0;i<G.vexnum;i++){
        dist[i]=GetArcWeight(G,v0,i);
        set[i]=0;
        if(GetArcWeight(G,v0,i)<INFINITE)
            path[i]=v0;
        else
            path[i]=-1;                 //v0->vi不经过任何顶点，可能不通或者是自身
    }//初始化完毕
    dist[v0]=0;
    set[v0]=1;                          //v0加入路径
    path[v0]=-1;
    //更新最短路径
    for(int i=0;i<G.vexnum;i++){
        min=INFINITE;
        //选出v0到剩余顶点中的一条最短路径
        for(int j=0;j<G.vexnum;j++)     //选出v0到剩余顶点中最短的一条路径
            if(set[j]==0&&dist[j]<min){
                min=dist[j];
                v=j;
            }
        set[v]=1;                       //将该顶点v加入路径
        //判断v加入后是否会造成其他顶点到v0的更短路径，是就更新，并将v作为这些点的前继结点
        for(int j=0;j<G.vexnum;j++){
            if(set[j]==0 && (min+GetArcWeight(G,v,j))<dist[j]){
                dist[j]=min+GetArcWeight(G,v,j);
                path[j]=v;
            }
        }
    }
    //用栈出路径
    for(int i=0;i<G.vexnum;i++){
        if(v0!=i){
            printf("从v%d到v%d的最短路径为：",v0,i);
            if(path[i]==-1||dist[i]==INFINITE)
                printf("∞");
            else{
                int k=i;
                S.push(k);
                while (path[k]!=-1){        //路径上的点入栈
                    S.push(path[k]);
                    k=path[k];
                }
                while (!S.empty()){         //出栈输出
                    int u=S.top();
                    S.pop();
                    printf("v%d ",u);
                }
                printf("，最短路径长度为：%d\n",dist[i]);
            }
        }
    }
}
#include "savetree.h"
///————————————————————————————————————————————————————————————————————————
int main(){
    ALGraph G,g;
    printf("实验8测试用图，书P7.3无向图:\n");
    printf("实验8测试用图7.3,结点数为%d\n",P73VER_NUM);
    Create_Graph(&G);
    AddVertex(&G);
    //构建弧
    ADDArcsforExp8(&G);
    printG(&G);
    saveGraph(&G,"sg.html");
    BFS(&G,4);
    DFS(&G,4);
//测试Dijkstra算法
    Create_Graph(&g);
    AddVertex(&g);
    ADDArcsforDijkstra(&g);
    printG(&g);
    saveGraph(&g,"sg.html");
    DijkstraPath(g,0);
}
