//
// Created by Chen Wang on 2020/11/10.
//

#ifndef EXP5_BITREEADT_TREEOPERATION_H
#define EXP5_BITREEADT_TREEOPERATION_H
#include <stdlib.h>
#include <stdio.h>
#endif //EXP5_BITREEADT_TREEOPERATION_H

//定义存放数据的类型,假设是可以存放多个数据的结构体类型
typedef struct{
    int id;
    int value;
} ElemType;

//定义二叉树的结点
typedef struct tnode{
    ElemType data;
    struct tnode *lchild;
    struct tnode *rchild;
} bNode;

//查找给定结点p的父节点 , t非空时，表示已经找到，就不需要递归调用，直接返回
bNode *parent(bNode *root,bNode *p);

//由结点的id（可以改成其它数据域的值）查找结点，返回结点指针 。(先序遍历）
bNode *locateByID(bNode *root, int id);

//采用后序遍历销毁树。思考：可以用先序遍历吗？不可以
void DestroyBTree(bNode *root);

//由结点的value查找结点，返回结点指针(先序遍历）
bNode *LocateByValue(bNode *root, int value);

//删除节点value=x的节点及其子树
void DeleteSubTreeByValue(bNode* root,int value);

//给定id1和id2，求其最近共同祖先，节点的id具有唯一性,且不允许两者id有直系关系
//两层循环，找父亲结点比较
bNode *NearestAncstByID(bNode *root,int id1,int id2);

//编写递归算法，求二叉树节点中最大的value和最小的value之差
int DiffValue(bNode *root);

//给定value值，求从根节点到value值节点的路径，用“左右右...”的左右孩子指针标记从根到节点的路径
//返回需要路径的步数，value是根结点本身返回0，没有此value结点返回-1；
int FindPathbyValue(bNode *root,int value);