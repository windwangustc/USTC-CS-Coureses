//
// Created by Chen Wang on 2020/11/23.
//

//文件名："HfmTree.cpp"
#include "HfmTree.h"
using namespace std;

int _HTNode_Compare(HTNode * e1, HTNode *e2)
{
    /*
    .	实现 SortedList 类的 compare 接口
    */
    if (e1->weight > e2->weight)
        return 1;
    else if (e1->weight == e2->weight)
        return 0;
    else
        return -1;
}

HfmTree::HfmTree()
{
    /*
    .	无参构造
    */
    //初始化变化集合链表
    this->varySet = new SortedList<HTNode>();
    this->varySet->Init(_HTNode_Compare, this->varySet->_ASC);
    //初始化原始集合链表
    this->originSet = new SortedList<HTNode>();
    this->originSet->Init(_HTNode_Compare, this->originSet->_ASC);
    //初始化哈夫曼树 及 叶节点数
    this->root = NULL;
    this->leafNum = 0;
}

void HfmTree::Init(string &s)
{
    /*
    .	初始化字符串，并构建哈夫曼树
    */
    //1.字符频率统计
    _Arr_StatisticCharFreq(s);
    //2.创建权值集合单链
    _CreateWeightSet();
    //3.创建哈夫曼树
    _CreateHfmTree();
    //4.生成哈夫曼编码
    _GenerateHfmCode();
}

void HfmTree::_Arr_StatisticCharFreq(string &s)
{
    /*
    .	统计字符频率
    */
    //指针 p 指向词频数组
    int * p = this->charFreqArr;
    int sum = s.length();	//总字符数
    char c = '\0';
    //词频统计
    for (int i = 0; i < (int)s.length(); i++)
    {
        c = s[i];
        p[c - this->_START_C]++;	//52个字符（a-z|A-Z）数组基底 0 为 'a'
    }
    //词频数组 归一化
    for (int i = 0; i < this->_ARR_SIZE; i++)
    {
        p[i] = (int)(p[i] * this->_MAGNIFICATION / sum);	//放大 100 倍（若某些字符权重太小，可扩大倍数）
    }
}

void HfmTree::_CreateWeightSet()
{
    /*
    .	构建哈夫曼树
    */
    //哈夫曼结点变量
    HTNode * node = NULL;
    //指针 p 指向词频数组
    int * p = this->charFreqArr;
    //遍历词频数组
    for (int i = 0; i < this->_ARR_SIZE; i++)
    {
        if (p[i] == 0)
            continue;
        //初始化 树结点
        node = new HTNode;
        node->data = (char)(i + this->_START_C);	//取字符
        node->weight = p[i];					//取权重
        node->parent = NULL;
        node->lchild = NULL;
        node->rchild = NULL;
        //顺序插入 权重集合单链表
        this->varySet->Insert(node);
    }
    //显示集合
    this->varySet->Display();
}

void HfmTree::_CreateHfmTree()
{
    /*
    .	创建哈夫曼树
    */
    //初始化 树结点
    HTNode *first = NULL, *second = NULL, *newNode = NULL;
    //权值集合 元素结点数 只剩一个时，结束
    while (this->varySet->Length() > 1)
    {
        //获取并删除 权值集合前两个元素 （集合升序排列，前两个为权值最小）
        first = this->varySet->Delete(1);
        second = this->varySet->Delete(1);
        //构建 新权值 根结点，并初始化
        newNode = new HTNode;
        newNode->data = '\0';
        newNode->weight = first->weight + second->weight;	//权值相加
        newNode->parent = NULL;
        newNode->lchild = first;
        newNode->rchild = second;
        //赋值 两个结点的 双亲
        first->parent = newNode;
        second->parent = newNode;
        //并将新结点 顺序插入集合，并显示集合
        this->varySet->Insert(newNode);
        this->varySet->Display();
        //将删除的两个元素结点（非后建的根结点），加入到 初始集合 中，并显示
        if (first->data != '\0')
            this->originSet->Insert(first);
        if (second->data != '\0')
            this->originSet->Insert(second);
        this->originSet->Display();
    }
    //取权重集合链 第一个元素 作为 哈夫曼树根
    this->root = this->varySet->Delete(1);
}

void HfmTree::_GenerateHfmCode()
{
    /*
    .	生成哈夫曼编码
    */
    HTNode * p = NULL, *q = NULL;
    char c = '\0';
    //遍历叶子结点（初始权重集合）
    for (int i = 0; i < this->originSet->Length(); i++)
    {
        //获取叶结点
        p = this->originSet->Get(i + 1);
        //获取字符
        c = p->data;
        //从叶节点 到 根 的遍历
        while (p->parent != NULL)
        {
            //q 取 p 的根结点
            q = p->parent;
            if (q->lchild == p)
                this->charCodeArr[c - this->_START_C] = "0" + this->charCodeArr[c - this->_START_C];
            else
                this->charCodeArr[c - this->_START_C] = "1" + this->charCodeArr[c - this->_START_C];
            //p 向根移动
            p = p->parent;
        }
        //置空 游走指针
        p = NULL;
        q = NULL;
    }
}

void HfmTree::HfmCodeDisplay()
{
    /*
    .	显示哈夫曼编码
    */
    HTNode *p = NULL;
    for (int i = 0; i < this->originSet->Length(); i++)
    {
        p = this->originSet->Get(i + 1);
        cout << "(" << p->data << ":" << p->weight << ":" << this->charCodeArr[p->data - this->_START_C] << ")" << endl;
    }
}

string HfmTree::Encoding(string s)
{
    /*
    .	编码
    */
    //初始化编码字符串
    string encodingStr = "";
    //遍历字符集
    for (int i = 0; i < (int)s.length(); i++)
    {
        encodingStr = encodingStr + this->charCodeArr[s[i] - this->_START_C];
    }
    return encodingStr;
}

string HfmTree::Decoding(string s)
{
    /*
    .	解码
    */
    //初始化 解码字符串
    string decodingStr = "";
    //初始化结点指针：p 指向哈夫曼树根结点
    HTNode *p = this->root;
    //初始化 编码： 0 1
    int code = 0;
    //遍历码串
    for (int i = 0; i < (int)s.length(); i++)
    {
        //从根 遍历，按码串路径 寻叶子结点
        while (p->lchild != NULL && p->rchild != NULL)
        {
            //取字符码 0 或 1，转换成 整型
            code = s[i] - '0';
            // 0|左子树  1|右子树
            if (code == 0)
                p = p->lchild;
            else
                p = p->rchild;
            //自增 i
            i++;
        }
        //抵消一次自增
        i--;
        //叶节点字符拼接
        decodingStr = decodingStr + p->data;
        //指针 p 置到 根结点
        p = this->root;
    }
    return decodingStr;
}

int main()
{
    //利用 字符集 s 构建哈夫曼树
    string s1 = "aaacdeefcccceeggghhh";
    HfmTree * t = new HfmTree();
    t->Init(s1);
    t->HfmCodeDisplay();
    //在构造的哈夫曼树基础上，测试 编码 解码
    string s2 = t->Encoding(s1);
    cout << "原文：" << s1 << endl;
    cout << "码文：" << s2 << endl;
    cout << "解码文：" << t->Decoding(s2) << endl;
    return 0;
}