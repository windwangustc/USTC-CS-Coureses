//
// Created by Chen Wang on 2020/11/23.
//


//文件名："SortedList.h"
#pragma once
#ifndef SORTEDLIST
#define SORTEDLIST
#include <string>
#include <iostream>
using namespace std;

/*
.	排序链表 类模板实现
*/
//链表结点
template <typename ElemType>
struct SLNode
{
    ElemType * e;	//数据域
    SLNode<ElemType> * next;		//链域
};

template <typename ElemType>
class SortedList
{
private:
    SLNode<ElemType> * head;					//头结点
    int length;									//链长
    int (*compare)(ElemType *, ElemType *);		//比较 函数指针
    int sortedType;								//排序类型

public:
    static const int _ASC = 1;				//排序类型：升序标识
    static const int _DESC = 0;				//排序类型：降序标识

    SortedList();						//无参构造
    void Init(int(*compare)(ElemType *, ElemType *), int sortedType);	//初始化

    int Length();						//获取链表长度
    void Insert(ElemType *e);			//顺序插入一个元素
    ElemType * Get(int index);			//获取第 index 个元素
    ElemType * Delete(int index);		//删除第 index 个元素
    void Display();						//显示链表
};

template <typename ElemType>
SortedList<ElemType>::SortedList()
{
    /*
    .	有参构造
    */
    //初始化头结点
    this->head = new SLNode<ElemType>;
    this->head->e = NULL;
    this->head->next = NULL;
}

template <typename ElemType>
void SortedList<ElemType>::Init(int(*compare)(ElemType *, ElemType *), int sortedType)
{
    /*
    .初始化：
    .	参数：
    .		1.int(*compare)(ElemType *, ElemType *): 比较函数（）
    .			其返回值：-1|小于 0|等于 1|大于
    .		2.int sortedType: 排序类型
    .			取值：_ASC 或 _DESC
    */
    //比较函数 初始化
    this->compare = compare;
    //排序类型
    this->sortedType = sortedType;
}

template <typename ElemType>
int SortedList<ElemType>::Length()
{
    /*
    .	获取链表长度
    */
    return this->length;
}

template <typename ElemType>
void SortedList<ElemType>::Insert(ElemType *e)
{
    /*
    .	顺序插入一个元素
    */
    //初始化  指针 p 为第一个结点，q 指向其前驱结点
    SLNode<ElemType> *p = this->head->next, *q = this->head;
    SLNode<ElemType> *newNode = NULL;
    //按 升序或降序 顺序寻找符合 compare 比较条件的 结点位置
    while (p != NULL && ((this->sortedType == this->_ASC && compare(e, p->e) > 0)
                         || (this->sortedType == this->_DESC && compare(e, p->e) < 0)))
    {
        q = p;
        p = p->next;
    }
    //在 p 结点前 （q 结点后）插入
    newNode = new SLNode<ElemType>;
    newNode->e = e;
    newNode->next = p;
    q->next = newNode;
    //自增 链长
    this->length++;
}

template <typename ElemType>
ElemType * SortedList<ElemType>::Get(int index)
{

    /*
    .	获取第 index 个元素
    */
    //链表越界检查
    if (index < 1 || index > this->length)
    {
        cout << "索引越界！" << endl;
        return NULL;
    }
    //初始化链表索引
    int i = 1;
    //初始化 p 指向链表第一个结点
    SLNode<ElemType> *p = this->head->next;
    //第一个结点为空，返回 空
    if (p == NULL)
        return NULL;
    //遍历链表，查找第 index 个元素结点
    while (i < index)
    {
        p = p->next;
        i++;
    }
    return p->e;
}

template <typename ElemType>
ElemType * SortedList<ElemType>::Delete(int index)
{
    /*
    .	删除第 index 个元素
    */
    //链表越界检查
    if (index < 1 || index > this->length)
    {
        cout << "索引越界！" << endl;
        return NULL;
    }
    //初始化链表索引
    int i = 1;
    //初始化 p 指向链表第一个结点，p 的前驱结点 q
    SLNode<ElemType> *p = this->head->next, *q = this->head;
    ElemType *e = NULL;
    //第一个结点为空，返回 空
    if (p == NULL)
        return NULL;
    //遍历链表，查找第 index 个元素结点
    while (i < index)
    {
        q = p;
        p = p->next;
        i++;
    }
    e = p->e;
    //删除 p 指针处结点
    q->next = p->next;
    delete p;
    //自减 链长
    this->length--;

    return e;
}

template <typename ElemType>
void SortedList<ElemType>::Display()
{
    /*
    .	显示链表
    */
    SLNode<ElemType> *p = this->head->next;
    cout << "The sorted list is : ";
    if (p == NULL)
        cout << "NULL";
    while (p != NULL)
    {
        cout << p->e << " ";
        p = p->next;
    }
    cout << endl;
}

#endif // !SORTEDLIST

