#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_
//图/网络的实现: 不用指针，用若干大数组来实现（图的顺序存储方式）
#include <cstdio>
#include <cstdlib>
#include <cstring>
/***
 * 实现的基本操作包括，但是不限于：图的创建、删除顶点、增加顶点、增加边、删除边、查找顶点、修改边或顶点
 * 图的存储结构自行设计，同时设计实现一个复杂操作，验证上述基本操作，并说明采该存储结构实现这个复杂操作是否合适（向助教说明结构和操作是否适配）
 * ***/
//定义图的最大定点数和最大边数
#define MAXV 100
#define MAXE 20000

//顶点结构体
typedef struct{
    int id;  						//顶点编号
    int w;  	 					//顶点权值
    int degree;						//顶点的(出度）度,也是edges[]的有效元素个数
    long edges[MAXV];  				//边的编号
} Vertex;

//边/弧结点结构体
typedef struct{
    long id;   						//弧编号
    int head,tail; 					//head:弧头结点 id, tail:弧尾结点 id
    int weight;   						//weight of edge
} Edge;

//定义图结构体：包括图的基本信息，所有边的基本信息
//所有顶点的基本信息（每个顶点包括了它的连边信息，可以视为邻接表的一种实现）
typedef struct{
    Vertex v[MAXV];        //顶点数组
    Edge e[MAXE];       //边数组,供查询用
    bool directional;     //tail：有向图，f：无向图
    bool weighted;        //tail：带权图，f：等权图
    unsigned nv;      	//图的实际顶点数
    unsigned long ne;  	//图的实际边数
    //bool available_v[MAXV]; //标记有效的顶点 ，用于实现高效版本的插入、删除顶点，用于快速依据编号查找顶点的信息
    //bool available_e[MAXE]; //标记有效的边 ,用于实现高效版本的插入、删除边，用于快速依据编号查找边的信息
    //高效版本做法：id和顶点数组的下标，边数组的下标保持一致，当删除边或顶点时，这两个数组相应位置设置为false
    //查找信息时直接用下标，增加顶点或边时从头开始查找一个为使用的位置插入即可
} Graph;

#include "showgt.h"

//小工具，随机图生成器，生成边表存储在文件filename中
//生成一个随机图包括n个顶点，每个顶点和其它任何顶点连边的概率为p
int randGenGraph(int n, float p, bool weighted, bool directional, const char *filename, int seed){
    srand(seed);//确保生成的图可以重现
    //保存文件内容，文件大小不超过5Mbytes，不断把随机生成的图信息写入字符串content中
    char *content=(char *)malloc(5000000);
    if (!content){
        printf("分配存储空间失败\n");
        exit(0);
    }
    FILE *fp = fopen(filename,"wb");
    if (!fp){
        printf("生成图文件 %s 时出错!\n",filename);
        exit(0);
    }

    long offset = 0; //content当前的写入位置
    long ne = 0; //生成边数

    for(int i=0;i<n;++i)
        offset += sprintf(content+offset,"%3d %5d\n",i,rand()%100);//写入结点的编号以及结点随机权值

    int pn = (int)(10000*p);
    if (directional){//有向图
        for(int i=0;i<n;++i) //写入随机边
            for(int j=0;j<n;++j)
                if (i!=j && rand()%10000<pn){
                    offset += sprintf(content+offset,"%3d  %3d  %4d\n",i,j, weighted?rand()%100:1);
                    ne++;
                }
    }else{//无向图
        for(int i=0;i<n;++i) //写入随机边
            for(int j=i+1;j<n;++j)
                if (rand()%10000<pn){
                    offset += sprintf(content+offset,"%3d  %3d  %4d\n",i,j, weighted?rand()%100:1);
                    ne++;
                }
    }

    char ch[40];
    int sz = sprintf(ch,"%3d %7d %5s %5s\n",n,ne,weighted?"true":"false",directional?"true":"false");
    fwrite(&ch,sz,1,fp);	//写入图的顶点数目和边的数目
    fwrite(content,offset,1,fp); //写入图的顶点和边信息
    fclose(fp);
    free(content);
}

//从文件中读入图，在内存中生成图的表示
void initGraph(Graph *g,const char *gfile){
    FILE *fp = fopen(gfile,"r");
    if (!fp){
        printf("读取图数据文件出错\n");
        exit(0);
    }

    char bv[10];
    //读取图的基本信息 ,图的类型，顶点数，边数等
    fscanf(fp,"%u%lu%s",&(g->nv),&(g->ne),bv);
    g->weighted = strcmp(bv,"true")==0?true:false;
    fscanf(fp,"%s",bv);
    g->directional = strcmp(bv,"true")==0?true:false;

    //读取顶点信息：id，weight，degree； 没有 edges（读边信息时加上）
    for (int i=0;i<g->nv;++i){
        fscanf(fp,"%d%d",&(g->v[i].id),&(g->v[i].w));
        g->v[i].degree = 0;
        //printf("顶点 %d - 权 %d\n",g->v[i].id,g->v[i].weight);
    }

    //读取边信息：  g.e[i]: id,head,tail,weight; 同时每条边要修改顶点的edges和degree
    for(unsigned long i=0;i<g->ne;++i){
        fscanf(fp, "%u%u%d", &(g->e[i].head), &(g->e[i].tail), &(g->e[i].weight));
        printf("边:%u-%u => %d\n", g->e[i].head, g->e[i].tail, g->e[i].weight);
        g->e[i].id = i;
        unsigned h,t;
        h = g->e[i].head;							//编号为i的边，弧头h，弧尾t
        t = g->e[i].tail;
        g->v[h].edges[g->v[h].degree++] = i;  	//弧头顶点h的出边表记录边的编号i
        if (g->directional==false)      		//无向图的弧尾顶点t的边表也要记录边编号i
            g->v[t].edges[g->v[t].degree++]=i;  //顶点的度也要+1
    }
    fclose(fp);
}

//打印邻接表的信息
void printG(Graph *g){
    printf("图的基本信息：顶点数(%u)-边数(%u)-%s-%s\n",g->nv,g->ne,g->weighted?"加权图":"等权图",g->directional?"有向图":"无向图");
    for(int i=0;i<g->nv;++i){
        printf("ID(%u)-度(%d)-权(%d)-弧表 tail(eid|weight)：%u ",g->v[i].id,g->v[i].degree,g->v[i].w, g->v[i].id);
        for(int j=0;j<g->v[i].degree;++j){
            unsigned long e = g->v[i].edges[j];
            printf("-> %u(%d-%d) ", g->e[e].tail, g->e[e].id, g->e[e].weight);
        }
        printf("\n");
    }
}

//从图的顶点id找顶点数组的下标,低效率版本
inline int getVexIdx(Graph *g,int id){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id)
            return i;
    printf("input wrong vertex id in getVexIdx()!\n");
    exit(0);
}

//查找给定编号的顶点，返回其权值 .  inline 内联函数编译的时候直接展开 ，低效率实现版本
inline int getVexW(Graph *g, int id){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id)
            return g->v[id].w;
    printf("input wrong vertex id in getVexW()!\n");
    exit(0);
}

//查找给定编号的顶点，返回其度 ，低效率实现版本 （这里给的有id找边或权的某些信息，都是低效版本，用了一次遍历顶点或边的循环
inline int getVexDegree(Graph *g, int id){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id)
            return g->v[id].degree;
    printf("input wrong vertex id in getVexDegree()!\n");
    exit(0);
}

//通过权值查找顶点编号
inline int locateVex(Graph *g, int w){
    for(int i=0;i<g->nv;i++)
        if (g->v[i].w == w)
            return g->v[i].id;
    return -1;
}

//通过边权值查找边的编号
inline long locateEdge(Graph *g,int w){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].weight == w)
            return g->e[i].id;
    return -1;
}

//查找边的权值，低效率实现版本
inline int getEdgeW(Graph *g,long id){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].id == id)
            return g->e[i].weight;
    printf("input wrong edge id in getEdgeW()!\n");
    exit(0);
}

//查找边的下标，低效率实现版本
inline int getEdgeIdx(Graph *g,long id){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].id == id)
            return i;
    printf("input wrong edge id in getEdgeIdx()!\n");
    exit(0);
}

//返回边的两个顶点，低效率实现版本
inline void getVex2ByEdge(Graph *g, long id,int *h,int *t){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].id == id){
            *h = g->e[i].head;
            *t = g->e[i].tail;
        }
    printf("input wrong edge id in getVer2ByEdge()!\n");
    exit(0);
}

//查找顶点的所有边,返回边数为*ne，和边数组的首地址 ，低效率版本
inline long *getEdgesByNode(Graph *g,int id, int *ne){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id){
            *ne = g->v[i].degree;
            return g->v[i].edges;
        }
    printf("input wrong vertex id in getEdgesByNode()!\n");
    exit(0);
}

//设置顶点权值 ，低效率版本
inline void setVexW(Graph *g,int id, int w){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id)
            g->v[i].w = w;
    printf("input wrong vertex id in setVexW()!\n");
    exit(0);
}

//设置边权值 ，低效率版本
inline void setEdgeW(Graph *g,int id, int w){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].id == id)
            g->e[i].weight = w;
    printf("input wrong edge id in setEdgeW()!\n");
    exit(0);
}

//增加一条边
void addEdge(Graph *g, int h, int t, int w){
    if (h<0 || t <0 || h>=g->nv || t>=g->nv){
        printf("wrong vertex id in addEdge()!\n");
        exit(0);
    }
    //修改边数组
    long ne = g->ne;
    g->e[ne].head = h;
    g->e[ne].tail = t;
    g->e[ne].weight = w; 		//设置边权值
    g->e[ne].id = ne; 	//设置边id
    g->ne += 1;
    //修改头尾两个顶点
    g->v[h].edges[g->v[h].degree++]=ne;
    if (g->directional==false)
        g->v[t].edges[g->v[t].degree++]=ne; //无向图需要修改弧尾结点的信息
}

//新增加一个顶点，只是一个孤立的顶点
void addVex(Graph *g, int w){
    g->v[g->nv].id = g->nv;
    g->v[g->nv].w = w;
    g->v[g->nv].degree = 0;
}

//删除指定id的边 ，将边数组的最后一条边移动到g->e数组被删除的位置
void deleteEdge(Graph *g, int id){
    for(int i=0;i<g->ne;++i)
        if (g->e[i].id == id){
            g->ne--;
            int ne = g->ne;
            int h = g->e[i].head;
            int t = g->e[i].tail;
            g->e[i].head = g->e[ne].head;
            g->e[i].tail = g->e[ne].head;
            g->e[i].id = g->e[ne].id;
            g->e[i].weight = g->e[ne].weight;
            for(int j=0;j<g->v[h].degree;j++)
                if (g->v[h].edges[j]==id)
                    g->v[h].edges[j] = g->v[h].edges[--(g->v[h].degree)];
            if (g->directional==false){//无向图还需要删除尾结点的边
                for(int j=0;j<g->v[t].degree;j++)
                    if (g->v[t].edges[j]==id)
                        g->v[t].edges[j] = g->v[t].edges[--(g->v[t].degree)];
            }
        }
    printf("wrong edge id in deleteEdge()!\n");
    exit(0);
}

//删除顶点
void deleteVex(Graph *g,int id){
    for(int i=0;i<g->nv;++i)
        if (g->v[i].id == id){
            for (int j=0;j<g->v[i].degree;++j){
                int eid = g->v[i].edges[j];
                deleteEdge(g,eid);
            }//删除顶点id的所有边
            //删除顶点本身,将最后一个结点移动过来覆盖第i个结点
            int nv = g->nv-1;
            g->v[i].id = g->v[nv].id;
            g->v[i].w = g->v[nv].w;
            g->v[i].degree = g->v[nv].degree;
            for(int j=0;j<g->v[i].degree;++j)
                g->v[i].edges[j]= g->v[nv].edges[j];
            g->nv--;
        }

    printf("wrong vertex id in deleteVex()!\n");
    exit(0);
}

int main(){
    int seed = 121;
    randGenGraph(10,0.2,false, false,"g1.txt",seed);
    Graph *g = (Graph *)malloc(sizeof(Graph));

    printf("size of g = %ld\n",sizeof(Graph));
    initGraph(g,"g1.txt");
    printG(g);
    saveGraph(g,"sg.html");

}


#endif
