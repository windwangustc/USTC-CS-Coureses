//
// Created by Chen Wang on 2020/11/23.
//

#include "SortedList.h"
#include "SortedList.h"
using namespace std;

struct Node
{
    int a;
    int b;
    friend ostream & operator <<(ostream& out, Node *p)
    {
        /*
        .	友元函数重载输出操作符，实现对象输出
        */
        out << p->a;
        return out;
    }
};

int compare(Node * e1, Node *e2)
{
    if (e1->a > e2->a)
        return 1;
    else if (e1->a == e2->a)
        return 0;
    else
        return -1;
}

int SortedListmain()
{
    Node *n1 = new Node{ 1, 1 };
    Node *n2 = new Node{ 2, 2 };
    Node *n3 = new Node{ 3, 3 };

    SortedList<Node> *list = new SortedList<Node>();
    list->Init(compare, list->_ASC);	//初始化为升序，实现 compare 接口
    //乱序插入三个元素
    list->Insert(n2);
    list->Insert(n3);
    list->Insert(n1);
    //测试 三个元素是否升序排列
    list->Display();
    //测试 链表索引越界
    list->Get(0);
    list->Get(4);
    //测试 获取第一个元素，并删除
    cout << "第一个元素：" << list->Get(1) << endl;
    list->Delete(1);
    list->Display();

    return 0;
}

