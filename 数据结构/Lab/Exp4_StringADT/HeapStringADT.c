//
// Created by Chen Wang on 2020/11/1.
//
/*  基本操作包括(但不限于):
        模式匹配:定位子串的位置，要求使用KMP算法实现
*/

#include "HeapStringADT.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *ch;           //若是非空串，则按串长分配存储区，否则ch为NULL
    int length;
}HString;

//初始化空串
void InitString(HString *T)
{
    T->ch = NULL;
    T->length = 0;
}

//初始化分配串
Status StrAssign2(HString* S, char* chars){
    if (S->ch)
        free(S->ch);
    int len=mystrlen(chars);    //求chars的长度
    if(!len){                   //空串
        S->ch=NULL;
        S->length=0;
    }
    else{
        S->ch = (char*)malloc(len * sizeof(char));        //分配空间
        if(!S->ch)
            exit(OVERFLOW);
        for (int i=0;i<len;i++)
            S->ch[i]=chars[i];
        S->length=len;
    }
    return OK;
}

//求串长
int StrLength2(HString S){
    return S.length;
}

//销毁串
Status DestroyString2(HString* S){
    free(S->ch);
    return OK;
}

//清空串
Status ClearString2(HString* T){
    if(T->ch){
        free(T->ch);
        T->ch=NULL;
    }
        T->length=0;
    return OK;
}

//求子串:返回某个起始位置为pos的某长度为len的子串
Status SubString2(HString* Sub,HString T,int pos,int len)
{
    if( pos<0 || pos>T.length || len < 0 || len > T.length-pos+1) {
        printf("输入的位置或长度有误！\n");
        return ERROR;
    }
    if(Sub->ch)
        free(Sub->ch);
    if (len==0){
        Sub->ch=NULL;
        Sub->length=0;
    }
    else{
        Sub->ch=(char*)malloc(len*sizeof(char));
        Sub->length=len;
        for (int i=0;i<len;i++,pos++)                       //S1传入T中
            Sub->ch[i]=T.ch[pos-1];
    }
    return OK;
}

//拼接:拼接两个串,用T保存
Status StrConcat2(HString* T,HString S1,HString S2){
    if (T->ch)                      //释放旧空间
        free(T->ch);
    T->ch=(char*)malloc((S1.length+S2.length)*sizeof(char));    //分配空间
    if(!T->ch)
        exit(OVERFLOW);
    T->length=S1.length+S2.length;
    for (int i=0;i<S1.length;i++)                       //S1传入T中
        T->ch[i]=S1.ch[i];
    for (int i=S1.length,i2=0;i<T->length;i++,i2++)     //S2传入T中
        T->ch[i]=S2.ch[i2];
    return OK;
}

//在pos位置删除长度为len串
Status StrDelete(HString *S,int pos,int len ){
    if(!S->ch)
        return ERROR;
    if(pos < 1 || pos+len-1 > S->length || len<0)
        return ERROR;

    for(int i = pos-1;i+len<S->length;i++)
        S->ch[i] = S->ch[i+len];
    S->length -= len;
    S->ch = (char*)realloc( S->ch, S->length*sizeof(char)); //重新分配空间
    return OK;
}

//在串S的第pos个位置前插入串T
Status StrInsert(HString *S,int pos,HString T)
{
    int i;
    if(pos<1 || pos-1>S->length || T.ch==NULL)
        return ERROR;
    else
    {
        S->ch = (char*)realloc(S->ch, (S->length+T.length)*sizeof(char));
        if(!S->ch)
            exit(OVERFLOW);
        for(i=S->length-1;i>pos-2;i--)         //先移位腾出T.len的空间
            S->ch[i+T.length] = S->ch[i];
        for(i = 0;i<T.length;i++)
            S->ch[pos-1+i] = T.ch[i];
        S->length+=T.length;                   //更新长度
    }
    return OK;
}

//比较两个串是否完全相等，相等返回TURE,不相等返回False
int StrCompare(HString S,HString T) {
    int i;
    if(S.length!=T.length)
        return FALSE;
    for(i = 0;i<S.length && i<T.length;i++)
    {
        if(S.ch[i] != T.ch[i])
            return FALSE;
    }
    return TRUE;
}

//查找子串，返回子串所在的位置
int Index(HString S,HString T,int pos){
    int s,t,i;
    HString tmp;
    InitString(&tmp);

    if(pos > 0){
        s = StrLength2(S);
        t = StrLength2(T);
        i = pos;
        while(i+t-1 <= s){
            SubString2(&tmp,S,i,t);             //在S中截取一段与T等长的子串
            if(StrCompare(tmp,T) == TRUE)
                return i;
            else
                i++;
        }
    }
return 0;
}

//替换:Replace(&S, T, V),用V替换主串S中出现的所有与T相等的不重叠的子串
Status StrReplace2(HString* S, HString T, HString V) {
    int i = Index(*S,T,1);
    while(i != 0){
        StrDelete(S,i,StrLength2(T));
        StrInsert(S,i,V);
        i += V.length;
        i = Index(*S,T,i);
    }
    return OK;
}

//遍历:输出给定串
Status StrTraverse2(HString S,void (*strfunc)(unsigned char a))
{
    int i = 0;
    if(!S.length) {
        printf("String Length is zero!\n");
        return ERROR;
    }
    while(i < S.length)
    {
        (*strfunc)(S.ch[i]);
        i++;
    }
    printf("\n");
    return OK;
}

//print函数
void StrPrint2(unsigned char a){
    printf("%c",a);
}

void HeapStringtest(){
    HString S1,S2,T,Sub;
    InitString(&S1);
    InitString(&S2);
    InitString(&T);
    InitString(&Sub);
    StrAssign2(&S1,"helloworld");
    StrAssign2(&S2,"-DataStructure");

    StrConcat2(&T,S1,S2);
    StrTraverse2(T,StrPrint2);
    SubString2(&Sub,T,3,4);
    StrTraverse2(Sub,StrPrint2);

    StrReplace2(&T,Sub,S1);
    StrTraverse2(T,StrPrint2);

    DestroyString2(&S1);
    DestroyString2(&S2);
    DestroyString2(&T);
    DestroyString2(&Sub);
}