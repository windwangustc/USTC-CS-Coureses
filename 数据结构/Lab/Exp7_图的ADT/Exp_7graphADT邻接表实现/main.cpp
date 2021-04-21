#include <cstdio>
#include <cstdlib>
/**基于邻接表实现图的ADT：图的创建、删除顶点、增加顶点、增加边、删除边、查找顶点、修改边或顶点* **/

#define DEBUG 0
#define MAX_VERTEX_NUM 20
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
///————————————————————————————————————————————————————————————————————————
//创建一个空图
ALGraph *Create_Graph(ALGraph *G){
    printf("请输入图的种类标志(0=DG;1=DN;2=UDG;3=UDN):\n");
    scanf("%d", &G->kind);
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
//在顶点数组AdjList末尾增加若干个值为value的数据元素，返回当前结点个数
int AddVertex(ALGraph *G){
    int addnum;
    printf("需要添加几个结点？\n");
    scanf("%d",&addnum);
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
            int v;
            scanf("%d", &v);
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
//删除id1，id2之间所连的弧
int DeleteArc(ALGraph *G, int id1,int id2) {
    if ( LocateVexbyID(*G,id1)==-1 || LocateVexbyID(*G,id2)==-1) {
        printf("弧依附的顶点id不存在!\n");
        return G->arcnum;
    }
    ArcNode *tmp1=nullptr, *tmp2=nullptr;
    ArcNode *pre1 = nullptr;//前继结点
    ArcNode *pre2 = nullptr;
    for (ArcNode *p = G->vertices[id1].firstarc; p != nullptr; p = p->nextarc) {
        if (p->adjvex == id2) {
            tmp1 = p;   //保存这条弧的位置以便删除后释放空间
            if (pre1 == nullptr)//pre1=NULL，说明firstarc.adjvex就等于id2
                G->vertices[id1].firstarc = p->nextarc;
            else
                pre1->nextarc = p->nextarc;
        }
        else
            pre1 = p;
    }
    if(tmp1!=nullptr)
        free(tmp1);     //释放这条弧的空间

// 无向图删除邻接表中对称的弧
    if(G->kind==2||G->kind==3) {
     for (ArcNode *p = G->vertices[id2].firstarc; p != nullptr; p = p->nextarc) {
         if (p->adjvex == id1) {
             tmp2 = p;   //保存这条弧的位置以便删除后释放空间
             if (pre2 == nullptr)
                 G->vertices[id2].firstarc = p->nextarc;
             else
                 pre2->nextarc = p->nextarc;
         } else
             pre2 = p;
     }
     if (tmp2 != nullptr)
         free(tmp2);     //释放这条弧的空间
    }
    G->arcnum--;
    return G->arcnum;
}
///————————————————————————————————————————————————————————————————————————
//删除某个id的结点
int DeleteVex(ALGraph *G, int id){
    if(LocateVexbyID(*G,id)==-1){
        printf("未找到此结点，删除失败！\n");
        return G->vexnum;
    }
    //先删除与该结点关联的所有边
    for(int i=0;i<G->vexnum+DeletedVexNum;i++){
        if(i!=id)
            DeleteArc(G,id,i);
    }
    //删除该结点
    G->vertices[id].id=-1;  //顶点id置为-1表示不存在
    DeletedVexNum++;
    G->vexnum--;
    return G->vexnum;
}
///————————————————————————————————————————————————————————————————————————
//修改顶点的值或弧的权重
int changeVexValue(ALGraph *G, int id, int v){
    if(LocateVexbyID(*G,id)==-1){
        printf("未找到此结点，无法修改！\n");
        return -1;
    }
    for(int i=0;i< G->vexnum + DeletedVexNum; ++i){
        if(G->vertices[i].value==v){
            printf("重复的value，不允许！\n");
            return -1;
        }
    }
    for(int i=0;i< G->vexnum + DeletedVexNum; ++i){
        if(G->vertices[i].id==id){
            G->vertices[i].value=v;
            printf("结点value修改成功！\n");
            return G->vexnum;
        }
    }
}
//修改弧的weight
int changeArcValue(ALGraph *G, int start_id,int end_id,int ori_w,int w){
    if ( LocateVexbyID(*G,start_id)==-1 || LocateVexbyID(*G,end_id)==-1) {
        printf("弧依附的顶点id不存在!\n");
        return G->arcnum;
    }
    auto p= G->vertices[start_id].firstarc;
    for(;p!= nullptr;p=p->nextarc) {
        if (p->adjvex==end_id && p->weight == ori_w)
            p->weight = w;
    }
    //无向图，要修改对称的边
    if(G->kind==2||G->kind==3){
        auto q= G->vertices[end_id].firstarc;
        for(;q!= nullptr;q=q->nextarc) {
            if (q->adjvex==start_id && q->weight == ori_w)
                q->weight = w;
        }
    }
    return G->arcnum;
}

///————————————————————————————————————————————————————————————————————————

int main() {
    ALGraph G1;
    Create_Graph(&G1);
    //增加若干个顶点
    AddVertex(&G1);
    printG(&G1);
    //增加弧
    AddArc(&G1,0,1,90);
    AddArc(&G1,0,2,49);
    AddArc(&G1,0,3,999);
    AddArc(&G1,1,2,788);
    AddArc(&G1,1,3,55);
    printG(&G1);
#if 1
    //删除顶点0
    printf("删除某个顶点后：\n");
    DeleteVex(&G1,0);
    printG(&G1);
    //顶点1的value修改为10000
    printf("修改顶点value后：\n");
    changeVexValue(&G1, 1, 10000);
    printG(&G1);
    //删除结点顶点12之间的弧
    printf("删除一条弧后：\n");
    DeleteArc(&G1,1,2);
    printG(&G1);
#endif
    //修改弧的信息
    printf("修改弧的信息后：\n");
    changeArcValue(&G1,1,3,55,100);
    printG(&G1);
    return 0;
}
