//
// Created by Chen Wang on 2020/11/10.
//
#include "TreeOperation.h"

/*** 二叉树的其它基本操作  ***/
//查找给定结点p的父节点 , t非空时，表示已经找到，就不需要递归调用，直接返回
bNode *parent(bNode *root,bNode *p){
    bNode *t = NULL;
    if (!root || root==p) return NULL;//根无父节点，空树也没有p的父节点
    if ( p && (root->lchild==p || root->rchild==p))
        return root;
    if (root->lchild && !t)         //t=NULL说明没找到，递归左孩子
        t = parent(root->lchild,p);
    if (root->rchild && !t)         //t=NULL说明还没找到，递归右孩子
        t = parent(root->rchild,p);
    return t;
}

//由结点的id（可以改成其它数据域的值）查找结点，返回结点指针 。(先序遍历）
bNode *locateByID(bNode *root, int id){
    if (!root) return NULL;
    if (root->data.id==id) return root;
    bNode *t = locateByID(root->lchild,id);
    if (t) return t;
    return locateByID(root->rchild,id);
}

//采用后序遍历销毁树。思考：可以用先序遍历吗？不可以
void DestroyBTree(bNode *root){
/*    if(root->lchild)
        DestroyBTree(root->lchild);
    if(root->rchild)
        DestroyBTree(root->rchild);
    if(!root->lchild&&!root->rchild)
        free(root);*/
    if (root->lchild){
        DestroyBTree(root->lchild);
        root->lchild=NULL;}
    if (root->rchild) {
        DestroyBTree(root->rchild);
        root->rchild=NULL;}
    if (!root->lchild && !root->rchild) free(root);
}

//由结点的value查找结点，返回结点指针(先序遍历）
bNode *LocateByValue(bNode *root, int value){
    if (!root) return NULL;
    if (root->data.value==value) return root;
    bNode *t = LocateByValue(root->lchild,value);
    if (t) return t;
    return LocateByValue(root->rchild,value);
}

//删除节点value=x的节点及其子树
//算法思路，找到该value结点和它的父结点即可
void DeleteSubTreeByValue(bNode* root,int value){
    bNode *valueAddr=LocateByValue(root,value);       //找到value的地址
    if(!valueAddr){
        printf("\n没有这个结点，请重试!\n");
        return;
    }
    bNode *parentAddr=parent(root,valueAddr);           //找到这个value的父节点的地址
    if(!parentAddr){
        printf("\n不允许删除根结点，请直接使用销毁函数！\n");
        return;
    }
    printf("\n删除value==%d结点及其子树",value);
    if (parentAddr->lchild != valueAddr){               //左孩子的地址和value的地址不一样，说明value是右孩子
        DestroyBTree(valueAddr);                        //销毁右孩子
        parentAddr->rchild=NULL;                        //右孩子置成NULL
    }
    else{
        DestroyBTree(valueAddr);
        parentAddr->lchild=NULL;
    }
}

//给定id1和id2，求其最近共同祖先，节点的id具有唯一性,且不允许两者id有直系关系
//两层循环，找父亲结点比较
bNode *NearestAncstByID(bNode *root,int id1,int id2){
    if (!root) return NULL;
    for(bNode* i1=locateByID(root,id1);i1!=root;i1=parent(root,i1)){
        for( bNode* i2=locateByID(root,id2);i2!=root;i2=parent(root,i2)){
            if(i1==i2) {                //找到祖先
                printf("\n输入id1=%d和id2=%d最近共同祖先的id为%2d, value==%2d",id1,id2,i2->data.id,i2->data.value);
                return i2;
            }
        }
    }
    printf("\n最近共同祖先为根结点");
    return root;
}

//给定value值，求从根节点到value值节点的路径，用“左右右...”的左右孩子指针标记从根到节点的路径
int FindPathbyValue(bNode *root,int value){
    int countstep=0;
    bNode *valueAddr=LocateByValue(root,value);       //找到value的地址
    if(!valueAddr){                                   //没有此value的地址
        printf("\n没有此结点，查找路径失败，请重试！");
        return -1;
    }
    while(valueAddr!=root){
        valueAddr=parent(root,valueAddr);
        countstep++;
    }
    //开辟保存父结点ID的数组，类似栈的用途
    int n=countstep+1;                      //n为数组的大小，数组大小为步数+1，因为还需要保存value本身
    bNode *tmpAddr[n];
    valueAddr=LocateByValue(root,value);              //找到value的地址
    tmpAddr[n-1]=valueAddr;                           //value地址保存到数组末尾
    while(valueAddr!=root){
        valueAddr=parent(root,valueAddr);
        n--;
        tmpAddr[n-1]=valueAddr;                       //tmpId[0]保存根结点地址
    }
    printf("\n从根结点到该value=%d结点需要%d步，具体路径为：\n",value,countstep);
    for(int i=0;i<countstep;i++){
        //printf("\n%lx,value==%2d\n ",tmpAddr[i],tmpAddr[i]->data.value);
        if(tmpAddr[i]->lchild==tmpAddr[i+1]) printf("左 ");
        else  printf("右 ");
    }
    return countstep;
}


//编写递归算法，求二叉树节点中最大的value和最小的value之差
int MaxValue,MinValue;
void FindMaxMin(bNode *root) {
    if (root == NULL)
        return;
    else {
        if (root->data.value > MaxValue) {
            MaxValue = root->data.value;
        }
        if (root->data.value < MinValue){
            MinValue = root->data.value;
    }
        FindMaxMin(root->lchild);
        FindMaxMin(root->rchild);
    }
}
int DiffValue(bNode *root){
    MaxValue=MinValue=root->data.value;
    FindMaxMin(root);
    printf("\n二叉树节点中最大的value值为%d,最小的value值为%d,两者之差为:%d ",MaxValue,MinValue,MaxValue-MinValue);
    return MaxValue-MinValue;
}
