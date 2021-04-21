//
// Created by Chen Wang on 2020/9/29.
//

#include "LinkList_ADT.h"
#include <stdio.h>
#include <stdlib.h>

#define ElemType int

#define _DEBUG_ 1

typedef struct Lnode {      //typedef链表节点，其中包含一个数据，和一个指向下个节点的指针
    ElemType data;          //数据域
    struct Lnode *next;     //指针域
}LinkNode, *LinkList;       //LinkNode等价于Lnode；Linklist等价于*Lnode

LinkNode *first;
LinkNode *initList(){       //构造一个链表并为其分配头节点
    LinkNode *p = (LinkNode *)malloc(sizeof(LinkNode)); //初始化
    if (!p){
        printf("初始化分配头节点失败！\n");
        exit(0);
    }
    p->next = NULL;         //初始化(*p).next
    return p;               //返回值为初始化后的p
}

#ifdef _DEBUG_

LinkNode head;

void initList1(LinkNode *p){
    p = (LinkNode*)malloc(sizeof(LinkNode));
    if (!p){
        printf("初始化分配头节点失败！\n");
        exit(0);
    }
    p->next = NULL;         //初始化(*p).next
}

void initList2(){
    first = &head;
    first->next = NULL;
}
#endif

int ListLength(LinkList p){      //返回链表p的长度
    int count=0;
    while (p->next!= NULL){      //若非尾节点，执行循环
        count++;
        p = p->next;
    }
    return count;
}

int ListEmpty(LinkList p){      //返回链表节点状态，若非尾节点则返回1
    if (p->next!=NULL)
        return 1;
    return 0;
}

void ClearList(LinkList p){     //将p置为空表
    LinkNode *q;
    while (p->next!=NULL){
        q = p->next;
        p->next = q->next;      //q->next是空的
        free(q);
    }
}

void DestroyList(LinkList p){   //清空链表p并释放空间
    ClearList(p);
    free(p);
}

int GetElem(LinkList p,int i,ElemType *e){
    int k = 0;
    while (p=p->next){  //当p->非空
        k++;            //顺序访问方式
        if(k==i){
            *e=p->data; //p[i]的数据传入*e中
            return k;
        }
    }
    return 0;
}

LinkNode *LocateElem(LinkList p,ElemType e){
    while(p=p->next){
        if(p->data==e){
            return p;       //存在返回p，不存在返回null
        }
    }
    return NULL;
}

LinkNode *PriorElem(LinkList p,LinkNode *cur_e){
    for(;p->next;p=p->next){
        if(p->next==cur_e)      //若p的直接后继等于cur_e，则p就是cur_e的直接前驱
            return p;
    }
    return NULL;
}

LinkNode *NextElem(LinkList p,LinkNode *cur_e){
    return cur_e->next;         //直接返回下一节点即可
}

LinkNode *ListInsert(LinkList p,int i,ElemType e){      //在第i个位置"之前"插入元素e
    if (i<1) return NULL;
    for(;p;p=p->next)
        if (--i<1){     //寻找第i-1位置
            LinkNode *q = (LinkNode*)malloc(sizeof(LinkNode));
            if(!q){
                printf("插入节点时，分配空间失败！\n");
                exit(0);
            }
            q->next = p->next;  //插入操作，q->next指向p->next
            p->next = q;        //p->next指向q
            q->data = e;
            return q;
        }
    return NULL;
}

int ListDelete(LinkList p,int i,ElemType *e){       //删除第i个元素，用*e返回第i个元素的值
    if(i<0) return 0;
    LinkNode *q = p;    //头节点
    for (p=p->next;p;p=p->next){
        if(--i<1){  //寻找第i个元素
            q->next = p->next;
            *e=p->data;
            free(p);
            return 1;                               //删除成功返回1
        }
        q=p;
    }
    return 0;
}

void ListTraverse(LinkNode *p,void (*ptrFunc)(LinkNode *ptr)){      //遍历表的元素，遍历操作用(*ptrFunc)()完成
    printf("链表(表长=%d):",ListLength(p));
    while (p=p->next)   (*ptrFunc)(p);
    printf("\n");
}

void PrintLinkNode(LinkNode *p){       //输出p->data
    printf("%d,",p->data);
}

void Add2(LinkNode *p){
    p->data+=2;
    printf("+2,");
}

void DebugLinkNode(LinkNode *p){
    printf("节点-(*addr)=value :  ");
    printf("(*%lx)=%d\n",p,p->data);
}


int LinkList_test() {
    void (*ptrFunc)(LinkNode *p) = DebugLinkNode ; //指定*ptrFunc函数的操作

    first=initList();               //分配头节点

    ListTraverse(first,ptrFunc);    //遍历链表
    ListInsert(first,1,2);    //在位置1前插入数据2
    ListInsert(first,1,3);
    ListInsert(first,1,4);
    ListInsert(first,1,5);
    ListTraverse(first,ptrFunc);    //遍历链表
    ListInsert(first,1,6);
    ListInsert(first,1,7);
    ListInsert(first,1,8);
    ListInsert(first,1,9);
    ListTraverse(first,ptrFunc);    //遍历链表
    ListInsert(first,3,666);
    ListInsert(first,5,777);
    ListInsert(first,7,888);
    ListInsert(first,9,999);
    ListTraverse(first,ptrFunc);    //遍历链表

    ElemType ei;
    printf("读数据前ei为 %d --",ei);
    GetElem(first,10,&ei);
    printf("读数据后ei为 %d\n",ei);

    LinkNode *q = LocateElem(first,888);
    if (!q)
        printf("没有找到对应节点\n");
    else{
        q = PriorElem(first,q);  //查找q的前驱
        printf("找到节点的前驱为 %d --",q->data);
        printf("所找的节点为 %d --",q->next->data);
        if (q->next->next) {
            printf("找到节点的后继为 %d --", NextElem(first, NextElem(first, q))->data);
            //printf("找到节点的后继为 %d --", q->next->next->data);
        }
        printf("\n");
    }

    printf("删除前的值 %d --",ei);
    if (ListDelete(first,15,&ei)>0)
        printf("删除的值为 %d \n",ei);
    else
        printf("删除失败\n",ei);

    ListTraverse(first,ptrFunc);
    printf("删除前的值为%d\n",ei);
    if (ListDelete(first,10,&ei)>0)
        printf("删除的值为 %d \n",ei);
    else
        printf("删除失败\n",ei);

    printf("删除前的值为%d --",ei);
    if (ListDelete(first,6,&ei)>0)
        printf("删除的值为%d \n",ei);
    else
        printf("删除失败\n",ei);
    ListTraverse(first,ptrFunc);

    ptrFunc = Add2;
    printf("每个元素+2\n");
    ListTraverse(first,ptrFunc);
    printf("加完后新的链表：");
    ListTraverse(first,PrintLinkNode);

    DestroyList(first);         //销毁链表
    //ListTraverse(first,PrintLinkNode);
    return 0;
}
