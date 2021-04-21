#include <stdio.h>
#include "HeapStringADT.h"

/***采用两种不同的存储方式(定长顺序结构存储、堆分配存储和块链存储三选二)实现串的ADT，包括串模式匹配的KMP算法。
 *  基本操作包括(但不限于): 初始化:初始化串
                        销毁:销毁串，释放空间
                        清空:清为空串
                        求长度:返回串中的元素个数，称为串的长度
                        模式匹配:定位子串的位置，要求使用KMP算法实现
                        求子串:返回某个起始位置的某长度的子串 替换:Replace(&S, T, V)
                            其中S是主串，用V替换主串S中出现的所有与T相等的不重叠的子串
                        拼接:拼接两个串
                        遍历:输出给定串
 * ***/

//定长顺序存储表示
typedef unsigned char SString[MAXSTRLEN+1];

//求字符串长度，等于strlen
int mystrlen(const char *Str)
{
    int i;
    i=0;
    while((*Str++)!='\0'){      //从字符串第一个字符起计数，遇到字符串结束标志'\0’停止计数
        i++;
    }
    return i;
}

//初始化串,生成一个其值等于chars的串T,第一个元素为字符串长度
Status StrAssign(SString T,char* chars){
    for (int i=0;i<MAXSTRLEN+1;i++)
        T[i]='\0';
    T[0]=mystrlen(chars);
    for (int i=1;*chars!='\0';i++,chars++)
        T[i]=*chars;
    return OK;
}

//销毁串，定长数组没有销毁
Status DestroyString(SString S){
    return ERROR;
}

//清空串
Status ClearString(SString S){
    S[0] = 0;      //'\0'=0
    return OK;
}

//求串长
int StrLength(SString S){
    return S[0];
}

//模式匹配:定位子串的位置，要求使用KMP算法实现，返回子串的位置,定位失败返回
int next[MAXSTRLEN];

void next3(SString t,int next[]) { //求模式t的next数组
    int k, j = 2;
    next[0] = -1;
    next[1] = 0;
    while (j < t[0]) {
        k = next[j - 1];
        while (k > 0 && t[k+1] != t[j - 1+1])
            k = next[k];
        if (k <= 0)
            if (t[1] == t[j - 1+1])
                next[j] = 1;
            else
                next[j] = 0;
        else
            next[j] = k + 1;
        j++;
    }
    while(j<MAXSTRLEN){
        next[j]=-1;
        j++;
    }
}

int KMP_index(SString S, SString T){
    int i=0,j=0;
    while (i+1<=S[0] && j+1<=T[0]){
        if(j==-1 || S[i+1]==T[j+1]){
            i++;
            j++;
        }
        else
            j= next[j];
    }
    if(j+1>T[0])
        return i-T[0];
    else
        return -1;
}


//求子串:返回某个起始位置为pos的某长度为len的子串
Status SubString(SString Sub,SString T,int pos,int len)
{
    if( pos<1 || pos>T[0] || len < 0 || len > T[0]-pos+1 )
        return ERROR;
    for(int ct = 1,cnt = pos;cnt <= len+pos-1;ct++,cnt++)
        Sub[ct] = T[cnt];
    Sub[0] = len;
    return OK;
}

//替换:Replace(&S, T, V),用V替换主串S中出现的所有与T相等的不重叠的子串
//若有与t匹配的子串被替换，则返回TRUE，否则返回FALSE
Status StrReplace(SString S, SString T, SString V) {
    int i, j, k, p, pos, flag;
    i = 1;
    flag = 0;
    k = T[0] - V[0];        //串t与串v的长度差
    while (i <= S[0] - T[0] + 1) {
        j = 1;
        pos = i;            //记录当前模式串的位置
        while (j <= T[0]) {
            if (S[pos] == T[j]) {//模式串匹配
                ++j;
                ++pos;
            } else
                break;
        }
        if(j > T[0]){                            //模式串匹配成功，进行替换操作
            if(k == 0){                          //串t与串v的长度相等
                for(j = 1,p = i;j <= V[0];++p,++j){
                    S[p] = V[j];
                }
            }
            if(k > 0){                           //串t的长度大于串v的长度
                for(j = 1,p = i;j <= V[0];++p,++j){
                    S[p] = V[j];
                }
                for(p = pos;p <= S[0];++p){     //被替换子串后的元素往前移
                    S[p-k]=S[p];
                }
                S[0] = S[0] - k;                //当前主串长度改变
            }
            if(k < 0){                          //串t的长度小于串v的长度
                if((S[0] - k)>MAXSTRLEN){
                    printf("替换子串时超出最大长度！\n");
                    return ERROR;
                }
                for(p = S[0]; p >= i;--p){      //被替换子串后的元素往后移
                    S[p-k] =S[p];
                }
                for(j = 1,p = i;j <= V[0];++p,++j){
                    S[p] = V[j];
                }
                S[0] = S[0] - k;                //当前主串长度改变
            }
            flag = 1;
        }
        else
            ++i;                                 //模式串匹配不成功，模式串向后移
    }
    if(flag)
        return TRUE;
    else
        return FALSE;
}

//拼接:拼接两个串,若发生截断返回FALSE
Status StrConcat(SString T,SString S1,SString S2){
    int pos,i,uncut;
    if(S1[0]+S2[0]<=MAXSTRLEN){     //不需要截断
        T[0]=S1[0]+S2[0];
        for(pos=1;pos<=S1[0];pos++)
            T[pos]=S1[pos];
        for(i=1;i<=S2[0];i++,pos++)
            T[pos]=S2[i];
        uncut = TRUE;
    }
    else if(S1[0]<MAXSTRLEN){       //截断
        T[0]=MAXSTRLEN;
        for(pos=1;pos<=S1[0];pos++)
            T[pos]=S1[pos];
        for(i=1;pos<=MAXSTRLEN;i++,pos++)
            T[pos]=S2[i];
        uncut = FALSE;
    }
    else{
        T[0]=MAXSTRLEN;
        for(pos=1;pos<=S1[0];pos++)
            T[pos]=S1[pos];
        uncut = FALSE;
    }
    return uncut;
}

//print函数
void StrPrint(unsigned char a){
    printf("%c",a);
}

//遍历:输出给定串
Status StrTraverse(SString S,void (*strfunc)(unsigned char))
{
    int i = 1;
    while(i <= S[0])
    {
        (*strfunc)(S[i]);
        i++;
    }
    printf("\n");
    return OK;

}

int main() {
#if SWITCH
    SString S1,T,Sub,ConcatStr;
    StrAssign(S1,"abaaababc");
    StrAssign(T,"babc");

    next3(S1,next);
    printf("串T的next数组为\n");
    for(int i=0;i<MAXSTRLEN;i++)
        printf("%d,",next[i]);
    printf("\n模式串匹配：%d\n",KMP_index(S1,T));

    SubString(Sub,S1,3,3);
    StrTraverse(Sub,StrPrint);

    StrReplace(S1,T,Sub);
    StrTraverse(S1,StrPrint);

    StrConcat(ConcatStr,S1,T);
    StrTraverse(ConcatStr,StrPrint);
#else
    HeapStringtest();
#endif
    return 0;
}
