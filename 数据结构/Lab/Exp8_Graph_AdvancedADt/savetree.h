#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_

#include <stdlib.h>
#include <stdio.h>

//下面代码仅包含在bitree.cpp文件中才会生效，被编译
#ifdef _BITREE_CPP_

/*** 以下代码用于将树（二叉链表）写入文件，然后用浏览器打开名为filename文件即可查看树/图 ***/
//先序遍历将根结点的边入文件fp,输出时value可以改成id，Lc和Rc分别表示边上的权值，这里用于标识左右孩子
//source: %d 父节点信息； target: %d 子节点信息; rela: xx 边上权值信息； type: resolved 用处不清楚
void writeTree(bNode *root,FILE *fp){
	if (root){
		if (root->lchild){
			fprintf(fp,"{source: '%d', target: '%d', 'rela': 'Lc', type: 'resolved'},\n",root->data.value,root->lchild->data.value);
			writeTree(root->lchild,fp);
		}
		if (root->rchild){
			fprintf(fp,"{source: '%d', target: '%d', 'rela': 'Rc', type: 'resolved'},\n",root->data.value,root->rchild->data.value);
			writeTree(root->rchild,fp);
		}
	}
}

//调用此函数即可生成filename文件(文件名用.html结尾），用chrome浏览器查看
void saveTree(bNode *root,const char filename[]){
	FILE *fp = fopen(filename,"wb");
	if (!fp) {
		printf("打开写入文件出错！\n");
		exit(0);
	}

	//读取头部文件，二进制方式写入filename文件
	FILE *fh = fopen("head.txt","rb");
	fseek(fh,0,SEEK_END);
	long fsize = ftell(fh);
	rewind(fh);
	unsigned char *dataArray;
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
	free(dataArray);
	fclose(fh);

	//将树的信息写入filename文件
	fprintf(fp,"\n\nvar links = \n[\n");
	writeTree(root,fp);
	fprintf(fp,"\n];");

	//读取尾部文件，二进制方式写入filename文件
	fh = fopen("tail.txt","rb");
	fseek(fh,0,SEEK_END);
	fsize = ftell(fh);
	rewind(fh);
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
	free(dataArray);
	fclose(fh);

	fclose(fp);
}
#endif

//下面代码仅包含在humantree.cpp文件中才会生效和被编译
#ifdef _HUFFMANTREE_CPP_
void writeTree(HTNode tail[],int ridx,FILE *fp){
	if (ridx>=0 && tail[ridx].weight>0){
		if (tail[ridx].lchild>=0){
			fprintf(fp,"{source: '%d-%d', target: '%d-%d', 'rela': '0', type: 'resolved'},\n",ridx,tail[ridx].weight,tail[ridx].lchild,tail[tail[ridx].lchild].weight);
			writeTree(tail,tail[ridx].lchild,fp);
		}
		if (tail[ridx].rchild>=0){
			fprintf(fp,"{source: '%d-%d', target: '%d-%d', 'rela': '1', type: 'resolved'},\n",ridx,tail[ridx].weight,tail[ridx].rchild,tail[tail[ridx].rchild].weight);
			writeTree(tail,tail[ridx].rchild,fp);
		}
	}
}

//调用此函数即可生成filename文件(文件名用.html结尾），用chrome浏览器查看
void saveTree(HTNode root[],int rt,const char filename[]){
	FILE *fp = fopen(filename,"wb");
	if (!fp) {
		printf("打开写入文件出错！\n");
		exit(0);
	}

	//读取头部文件，二进制方式写入filename文件
	FILE *fh = fopen("head.txt","rb");
	fseek(fh,0,SEEK_END);
	long fsize = ftell(fh);
	rewind(fh);
	unsigned char *dataArray;
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
	free(dataArray);
	fclose(fh);

	//将树的信息写入filename文件
	fprintf(fp,"\n\nvar links = \n[\n");
	writeTree(root,rt,fp);//m需要修改，如果256个字符没有全部出现
	fprintf(fp,"\n];");

	//读取尾部文件，二进制方式写入filename文件
	fh = fopen("tail.txt","rb");
	fseek(fh,0,SEEK_END);
	fsize = ftell(fh);
	rewind(fh);
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
	free(dataArray);
	fclose(fh);

	fclose(fp);
}

#endif

#ifdef _NETWORK_CPP_
//保存图数据为html文件，供浏览器查看
void writeGraph(ALGraph *g,FILE *fp){
    if (g->kind==DN||g->kind==UDN)
        for(int i=0;i<g->vexnum;++i){
            ArcNode *p=g->vertices[i].firstarc;
            if(g->kind==DN){
                while(p!=NULL){
                    fprintf(fp,"{source: '%d', target: '%d', 'rela': '%d', type: 'resolved'},\n",g->vertices[i].id,p->adjvex,p->weight);
                    p=p->nextarc;
                }
            }
            else{
                while(p!=NULL){
                    if(p->adjvex>g->vertices[i].id)
                        fprintf(fp,"{source: '%d', target: '%d', 'rela': '%d', type: 'resolved'},\n",g->vertices[i].id,p->adjvex,p->weight);
                    p=p->nextarc;
                }
            }
        }
    else
        for(int i=0;i<g->vexnum;++i){
            ArcNode *p=g->vertices[i].firstarc;
            if(g->kind==DG){
                while(p!=NULL){
                    fprintf(fp,"{source: '%d', target: '%d', 'rela': '', type: 'resolved'},\n",g->vertices[i].id,p->adjvex,p->weight);
                    p=p->nextarc;
                }
            }
            else{
                while(p!=NULL){
                    if(p->adjvex>g->vertices[i].id)
                        fprintf(fp,"{source: '%d', target: '%d', 'rela': '', type: 'resolved'},\n",g->vertices[i].id,p->adjvex);
                    p=p->nextarc;
                }
            }
        }
}

//与树的保存代码几乎完全意义，除了调用writeGraph()有差别
void saveGraph(ALGraph *g,const char filename[]){
    FILE *fp = fopen(filename,"wb");
    if (!fp) {
        printf("打开写入文件出错！\n");
        exit(0);
    }
    //读取头部文件，二进制方式写入filename文件
    FILE *fh = fopen("head.txt","rb");
    fseek(fh,0,SEEK_END);
    long fsize = ftell(fh);
    rewind(fh);
    unsigned char *dataArray;
    dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
    if (!dataArray) {
        printf("文件太大，内存不够，读入错误! %d\n",fsize);
        exit(0);
    }
    fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
    fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
    free(dataArray);
    fclose(fh);

    //将树的信息写入filename文件
    fprintf(fp,"\n\nvar links = \n[\n");
    writeGraph(g,fp);
    fprintf(fp,"\n];");

    //读取尾部文件，二进制方式写入filename文件
    if (g->kind==DG||g->kind==DN)//有向图
        fh = fopen("tail.txt","rb");
    else //无向图
        fh = fopen("tail1.txt","rb");
    fseek(fh,0,SEEK_END);
    fsize = ftell(fh);
    rewind(fh);
    dataArray = (unsigned char *)malloc(sizeof(unsigned char)*fsize);
    if (!dataArray) {
        printf("文件太大，内存不够，读入错误! %d\n",fsize);
        exit(0);
    }
    fread(dataArray,sizeof(unsigned char),fsize,fh); //读取文件
    fwrite(dataArray, sizeof(unsigned char),fsize,fp);//写入文件
    free(dataArray);
    fclose(fh);

    fclose(fp);

}
#endif


#endif

