/***
下次实验要求：针对第八章的小程序验证。
1. 参照作用域嵌套的那页ppt，要求打印出不同作用域X的值。
2. 运用名字空间和using声明指令的知识进行自主编程，要求体现名字空间和using用法，比如输出不同头文件中的相同变量名的变量值。
***/
#include <iostream>
#include "head1.h"
#include "head2.h"
using namespace std;

class test{
private:
    int x;
public:
    void printx(){
        test::x=4;
        cout<<"test类中的x值为："<<test::x<<endl;
    }
};

//第0层
int x=10;
int y=5;        //全局变量x，y

int func()
{
    //第一层，函数内作用域
    int x=8;		// func()内的局部变量x，初始化为7
    test V;
    {
        //第二层，局部作用域
        int x = y;	// another local x, initialized by the global
        x++;		// x=5+1=6
        //第三层，语句作用域 for循环
        for (int x=1;x<2;x++)
            cout<<"第三层内的x值为:"<<x<<endl;
        V.printx();         //输出类中x的值
        cout<<"第二层内的x值为:"<<x<<endl;
    }
    cout<<"第一层内的x值为:"<<x<<endl;
    return 0;
}

void func_2(){
    {
        using first::SameValue;
        cout << "first名字空间内的变量值为：" << SameValue << endl;
    }
    {
        using second::SameValue;
        cout << "second名字空间内的变量值为：" << SameValue << endl;
    }
}
int main() {
    func();
    cout<<"第0层内的x值为:"<<x<<endl;
    func_2();
    return 0;
}
