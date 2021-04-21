//
// Created by Chen Wang on 2020/11/23.
//

//文件名："HfmTree.h"
#pragma once                //该文件仅允许被include一次
#include "SortedList.h"		//链表排序头文件
#include <string>
using namespace std;
/*
.	三叉链表实现哈夫曼树及哈夫曼编解码
*/

//哈夫曼树结点三叉链表静态结构
struct HTNode
{
    char data;				//字符域
    int weight;				//权重
    HTNode * parent;		//双亲结点
    HTNode * lchild;		//左指针域
    HTNode * rchild;		//右指针域
    friend ostream & operator <<(ostream& out, HTNode *p)
    {
        //友元函数重载输出操作符，实现对象输出
        out << "(" << p->data << ":" << p->weight << ")";
        return out;
    }
};

class HfmTree
{
private:
    /*
    .	词频数组
    .	目前支持：英文字符（含大小写），共52个
    */
    static const int _ARR_SIZE = 52;			//词频数组大小
    static const char _START_C = 'a';			//词频数组 0 下标对应的 字符 'a'
    static const int _MAGNIFICATION = 100;		//词频放大倍数
    int charFreqArr[_ARR_SIZE]{0};				//词频数组（含大小写），初始化为 0
    string charCodeArr[_ARR_SIZE]{ "" };		//字符编码数组

    void _Arr_StatisticCharFreq(string &s);		//统计字符频率

    /*
    ,	权值集合排序单链表
    */
    SortedList<HTNode> * varySet;				//变化的权值集合 链表（用于构建哈夫曼树根结点的生成）
    SortedList<HTNode> * originSet;				//初始的权重集合 链表（用于存放 叶节点指针）

    /*
    .	哈夫曼树
    */
    HTNode * root;								//哈夫曼树根结点
    int leafNum;								//叶结点数
    void _CreateWeightSet();					//创建权值集合（排序单链表）
    void _CreateHfmTree();						//构建哈夫曼树
    void _GenerateHfmCode();					//生成哈夫曼编码

public:
    HfmTree();									//无参构造
    void Init(string &s);						//初始化字符串
    void HfmCodeDisplay();						//显示哈夫曼编码
    string Encoding(string s);					//编码
    string Decoding(string s);					//解码
};


