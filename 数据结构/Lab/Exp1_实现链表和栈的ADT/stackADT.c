//
// Created by Chen Wang on 2020/9/29.
//

#include "stackADT.h"

#include <stdio.h>
#include <stdlib.h>

#define STACK_INIT_SIZE 100
#define STACKINCREMENT 10

unsigned int_stacksize;     //预分配的栈空间大小
int *int_stacktop_ptr;      //栈顶指针
int *int_stackbase_ptr;     //栈底指针

/*下面用宏定义来实现栈的8个基本操作
 * ##表示把前后两个参数连接起来
 * gcc -E 文件名.cpp 只做预编译，用来粗略检查宏定义是否存在问题
 */

//栈初始化，stacktop=stackbase
#define initStack(stack) stack ## _stackbase_ptr = (stack*)malloc(STACK_INIT_SIZE*sizeof(stack));\
                         if (stack ## _stackbase_ptr){                                           \
                         stack ## _stacktop_ptr = stack ## _stackbase_ptr;                       \
                         stack ## _stacksize = STACK_INIT_SIZE;                                  \
                         }else exit(0)

//判断栈是否为空，是则返回1
#define stackEmpty(stack) stack ## _stacktop_ptr == stack ## _stackbase_ptr ? 1:0

//取得栈顶元素到e中，返回1
#define getTop(stack,e) stack ## _stacktop_ptr == stack ## _stackbase_ptr ? 0:(e=*(stack ## _stacktop_ptr-1),1)

//清空栈
#define clearStack(stack) stack ## _stacktop_ptr = stack ## _stackbase_ptr

//销毁栈
#define destroyStack(stack) free(stack ## _stackbase_ptr)

//求栈长度
#define stackLength(stack) stack ## _stacktop_ptr - stack ## _stackbase_ptr

//出栈操作
#define pop(stack,e) stack ## _stacktop_ptr == stack ## _stackbase_ptr ? 0:(e =*(--stack ## _stacktop_ptr),1)

//入栈操作
#define push(stack,e) if (stack ## _stacktop_ptr -stack ## _stackbase_ptr >= stack ## _stacksize){ \
                        stack ## _stackbase_ptr = (stack *)realloc(stack ## _stackbase_ptr,        \
                        sizeof(stack)*(stack ## _stacksize + STACKINCREMENT));                     \
                        if (! stack ## _stackbase_ptr) exit(0);                                    \
                        stack ## _stacktop_ptr = stack ## _stackbase_ptr + stack ## _stacksize;    \
                        stack ## _stacksize = stack ## _stacksize + STACKINCREMENT;}               \
                        *(stack ## _stacktop_ptr++) = e

//栈的遍历操作 stackTraverse（）忽略

/*定义其他基本类型的栈，比如字符栈或者结构体栈，只需要定义静态结构，基本操作重复利用上面的宏定义
 * 下面的例子分别定义了字符栈和结构体栈
 * */

unsigned char_stacksize;     //预分配的栈空间大小
int *char_stacktop_ptr;      //栈顶指针
int *char_stackbase_ptr;     //栈底指针

//结构体栈的静态部分，未测试
typedef struct node {
    int data[10];
    float x,y;
}tnode;

unsigned tnode_stacksize;     //预分配的栈空间大小
int *tnode_stacktop_ptr;      //栈顶指针
int *tnode_stackbase_ptr;     //栈底指针

//定义栈的ADT，目的是为了只用基本操作实现复杂功能，防止误操作
int stack_test() {
    initStack(int);
    initStack(char);
    initStack(tnode);//初始化栈

    int x;
    if (pop(int,x))
        printf("出栈成功 %d\n",x);
    else
        printf("栈空，不能出栈\n");//此时运行的话栈空，输出此条

    printf("栈中有%d个元素\n",stackLength(int));

    if(stackEmpty(int))
        printf("栈空，不能取栈顶\n");
    else
    if(getTop(int,x))
        printf("栈顶元素是 %d\n",x);

    push(int,4);
    push(int,5);
    printf("栈中有%d个元素\n",stackLength(int));

    if (pop(int,x))
        printf("出栈成功 %d\n",x);
    else
        printf("栈空，不能出栈\n");//此时运行的话输出出栈成功5

    printf("栈中有%d个元素\n",stackLength(int));

    if(stackEmpty(int))
        printf("栈空，不能取栈顶\n");
    else
    if(getTop(int,x))
        printf("栈顶元素是 %d\n",x);

    clearStack(int);
    printf("栈中有%d个元素\n",stackLength(int));
    return 0;
}

