/***
 * -------------Created by Chen Wang----------------
 *                Date：25/10/2020
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*高精度运算
实现实数x(-1024<x<1024)的加减乘运算，要求运算精确到2^(-n)，n是一个输入参数或预定义参数。 要求和提示如下：
        1.用十进制或二进制串（线性表）表示实数x
        2.构造ADT，具有读入十进制实数的功能，实现到N进制的转换（N的值可以输入时指定），实现加减乘三个基本操作
        3.实现一个复杂操作：单变量多项式求值。例如求函数f(x)=3x^3-x^2+4的值（输入为x=1.4, 精度为：n=200）。
        提示：在运行时解析输入的多项式字符串,系数不包含小数，分析出每项的系数和幂数，调用基本操作完成计算。
*/
#define MAXSIZE 100          //小数最大精度
#define Z_SIZE  50           //整数最大位数
int n=MAXSIZE;                     //精确到的小数位

//十进制数的静态结构
typedef struct decimal{
    char sign;                //符号为 正数为1，负数为-1
    char zhengshu[Z_SIZE];    //正数最大为1024，数组容量为4
    char xiaoshu[MAXSIZE];
    char xlen;                //小数位数，最大250位，用char足够
}dec_num,*dec_ptr;
//————————————————————————————————————————————————————————
void debugPrint(dec_num x,int n){
    int pos=0;
    if(x.sign==-1)
        printf("%c",'-');
    for(int k=0;k<Z_SIZE;k++) {
        if (x.zhengshu[k]!=0) {
            pos = k;
            break;
        }
    }
    for(;pos<Z_SIZE;pos++)
        printf("%d",x.zhengshu[pos]);
    printf(".");
    for(int k=0;k<n;k++)
        printf("%d",x.xiaoshu[k]);
    printf("\n");
}
//————————————————————————————————————————————————————————
//dec_num结构体初始化
void initDecNum(dec_ptr x){
    x->sign=1;              //默认为正
    x->xlen=0;              //默认0位
    memset(x->zhengshu,0,Z_SIZE*sizeof(char));
    memset(x->xiaoshu,0,MAXSIZE*sizeof(char));
}
//————————————————————————————————————————————————————————
//判断两个数绝对值大小函数,返回绝对值大的数，完全相等返回NULL
dec_ptr absCompare(dec_ptr a,dec_ptr b) {
    for (int k = 0; k < Z_SIZE; k++) {    //遍历整数部分
        if ((a->zhengshu[k] > b->zhengshu[k]))
            return a;
        else if ((a->zhengshu[k] < b->zhengshu[k]))
            return b;
        if (k == Z_SIZE - 1) {            //k到整数末尾还没比较出来，比较小数
            for (int m = 0; m < MAXSIZE; m++) {
                if ((a->xiaoshu[m] > b->xiaoshu[m]))
                    return a;
                else if ((a->xiaoshu[m] < b->xiaoshu[m]))
                    return b;
                if (m == MAXSIZE - 1)               //小数最后一位还没逼出来，两个数完全相等，符号相反
                    return NULL;                       //两个数完全相等，返回0
            }
        }
    }
}
//————————————————————————————————————————————————————————
//移位函数，小数点向前移len位
void move(dec_ptr x,int len){
    for(int i=MAXSIZE-1;i>=len;i--)
        x->xiaoshu[i]=x->xiaoshu[i-len];
    for(int i=len-1,j=Z_SIZE-1;i>=0;i--,j--) {
        if(j<0)
            x->xiaoshu[i]=0;
        else
        x->xiaoshu[i] = x->zhengshu[j];
    }
    for(int i=Z_SIZE-1;i>=0;i--) {
        if(i-len<0)
            x->zhengshu[i]=0;
        else
        x->zhengshu[i] = x->zhengshu[i-len];
    }
    x->xlen=x->xlen+len;     //更新小数位数
}
//————————————————————————————————————————————————————————
//从控制台读入string存放到dec_num结构体中
int decimal_input(dec_ptr x){   //从控制台将输入字符串存入十进制数组
    char input[Z_SIZE+MAXSIZE+1];
    int i=0,j,z;                      //z暂存输入数字的整数部分,j暂存x.xiaoshu[j]
    initDecNum(x);
    fflush(stdin);
    //printf("请输入十进制数: \n");
    fgets(input,Z_SIZE+MAXSIZE+1,stdin);         //从控制行读入最大为5+MAXSIZE的10进制字符串，以回车结束
    z=atoi(input);                      //先将输入字符的整数部分保存下来

    if(z>1024 || z<-1024) {
        printf("输入的数超出范围！\n");
        exit(255);
    }

    if(z<0){
        x->sign=-1;                      //正负标志为置成-1
        z=-1*z;                         //转成正数统一处理
    }
    x->zhengshu[Z_SIZE-4]= (char)(z/1000);              //整数部分依次存入x.zhengshu[]
    x->zhengshu[Z_SIZE-3]= (char)(z/100%10);
    x->zhengshu[Z_SIZE-2]= (char)(z/10%10);
    x->zhengshu[Z_SIZE-1]= (char)(z%10);

    while (input[i]!='.' && i<=4){      //寻找小数的位置
        i++;
    }
    for(i=i+1,j=0;j<MAXSIZE && input[i+1]!='\0';i++,j++) {   //小数部分存入
        x->xiaoshu[j] = input[i] - '0';
        if(x->xiaoshu[j]!=0)
            x->xlen=j+1;                    //记录小数的位数
    }
    //printf("输入的值为：");
    //debugPrint(*x,n);
    return 0;
}
//————————————————————————————————————————————————————————
//加法ADT(a+b)
dec_num add(dec_num a,dec_num b){
    dec_num tmp_res,tmp_low;               //存放加完的结果
    int t=0;                               //t存放进位
    initDecNum(&tmp_res);
    //ab同号的情况
    if (a.sign==b.sign) {
        tmp_res.sign = a.sign;
        for (int k = MAXSIZE - 1; k >= 0; k--) {              //小数部分加法
            tmp_res.xiaoshu[k] = (a.xiaoshu[k] + b.xiaoshu[k] + t) % 10;
            t = (a.xiaoshu[k] + b.xiaoshu[k]+t) / 10;
        }
        for (int k = Z_SIZE - 1; k >= 0; k--) {              //整数部分加法
            tmp_res.zhengshu[k] = (a.zhengshu[k] + b.zhengshu[k] + t) % 10;
            t = (a.zhengshu[k] + b.zhengshu[k]+t) / 10;
        }
    }
    //ab异号的情况，即减法。先判断绝对值大小，确定符号。
    else {
        //判断两个数的绝对值大小以确定tmp_res的符号
        if(absCompare(&a,&b)==NULL){                //ab绝对值完全相等，直接赋0
            initDecNum(&tmp_res);
            return tmp_res;
        }
        memcpy(&tmp_res,absCompare(&a,&b),sizeof(dec_num));   //tmp_res暂时保存绝对值大的数
        if (a.sign!=tmp_res.sign)                          //如果a的符号与tmp_res不同，说明a绝对值小
            memcpy(&tmp_low,&a,sizeof(dec_num));             //把a赋值给tmp_low，保持tmp_low为绝对值小的数
        else
            memcpy(&tmp_low,&b,sizeof(dec_num));

        for (int k = MAXSIZE - 1; k >= 0; k--) {              //小数部分减法
            if((tmp_res.xiaoshu[k] - tmp_low.xiaoshu[k])>=0)
                tmp_res.xiaoshu[k] = tmp_res.xiaoshu[k] - tmp_low.xiaoshu[k];
            else {
                tmp_res.xiaoshu[k] = tmp_res.xiaoshu[k] - tmp_low.xiaoshu[k]+10;
                if (k==0)
                    tmp_res.zhengshu[Z_SIZE-1] -=1;
                else
                    tmp_res.xiaoshu[k-1] -=1;
            }
        }
        for (int k = Z_SIZE - 1; k >= 0; k--) {              //整数部分减法
            if((tmp_res.zhengshu[k] - tmp_low.zhengshu[k])>=0)
                tmp_res.zhengshu[k] = tmp_res.zhengshu[k] - tmp_low.zhengshu[k];
            else {
                tmp_res.zhengshu[k] = tmp_res.zhengshu[k] - tmp_low.zhengshu[k]+10;
                tmp_res.zhengshu[k-1] -=1;
            }
        }
    }
    tmp_res.xlen = (a.xlen>=b.xlen)?a.xlen:b.xlen;    //小数位数位取大的
    return tmp_res;
}
//————————————————————————————————————————————————————————
//减法ADT(a-b)
dec_num minus(dec_num a,dec_num b){
    dec_num tmp_b;
    memcpy(&tmp_b,&b,sizeof(dec_num));
    tmp_b.sign=0-tmp_b.sign;
    return add(a,tmp_b);              // a-b=a+(-b);
}
//——————————————————————————————————————————————————————————
//乘法ADT(a*b)
dec_num multiply(dec_num a,dec_num b){
    int bz;                        //保存b的int型正数部分
    dec_num tmp_res;                 //存放加完的结果
    dec_num tmp_res1;                 //存放加完的结果
    dec_num tmp_res2;                 //存放加完的结果
    initDecNum(&tmp_res);
    initDecNum(&tmp_res1);
    initDecNum(&tmp_res2);
    dec_num tmp_a;                   //b的副本
    dec_num tmp_b;                   //b的副本
    memcpy(&tmp_a,&a,sizeof(dec_num));
    memcpy(&tmp_b,&b,sizeof(dec_num));
    tmp_a.sign=1;
    tmp_b.sign=1;

    for(int i=0;i<Z_SIZE;i++){       //ascii转化
        tmp_b.zhengshu[i]=tmp_b.zhengshu[i]+'0';
    }
    bz=atoi(tmp_b.zhengshu);
    while (bz!=0){          //先乘以b的整数部分
        tmp_res1=add(tmp_res1,tmp_a);
        bz--;
    }
    //将b的小数部分先化成整数计算，再用move函数后移b.xlen位
    int len,i;
    long long int bx=0;                  //保存b的int型正数部分,小数部分
    memcpy(&tmp_b,&b,sizeof(dec_num));
    for(len=tmp_b.xlen-1,i=0;len>=0;len--,i++){
        bx=bx+tmp_b.xiaoshu[len]*pow(10,i);
    }
    while (bx!=0){
        tmp_res2=add(tmp_res2,tmp_a);
        bx--;
    }
    move(&tmp_res2,b.xlen);
    tmp_res=add(tmp_res1,tmp_res2);

    if(a.sign!=b.sign)        //符号判定
        tmp_res.sign=-1;
    else
        tmp_res.sign=1;
    return tmp_res;
}
//——————————————————————————————————————————————————————————
//幂ADT,输入幂的大小m，返回x^m
dec_num power(dec_num x,int m){
    dec_num res;
    initDecNum(&res);
    if(m==0){
        res.zhengshu[Z_SIZE-1]=1;
        return res;
    }
    memcpy(&res,&x,sizeof(dec_num));
    if(m==1)
        return res;
    else{
        int k=m;
        while(k!=1){
            res=multiply(res,x);
            k--;
        }
        return res;
    }
}
//——————————————————————————————————————————————————————————
//N进制整数转换函数
void fun(int n,int m)
{
    if(n<m)
    {
        if(n<10)
            printf("%d",n);
        else
            printf("%c",n-10+'A');
    }
    else
    {
        fun(n/m,m);
        if(n%m<10)
            printf("%d",n%m);
        else
            printf("%c",n%m-10+'A');
    }
}
//——————————————————————————————————————————————————————————
//N进制转换函数
void decTobaseN(dec_num x,int N){
    dec_num res;
    initDecNum(&res);
    int z;
    for(int i=0;i<Z_SIZE;i++){       //ascii转化
        x.zhengshu[i]=x.zhengshu[i]+'0';
    }
    z=atoi(x.zhengshu);
    //整数部分转化
    printf("结果转化为%d进制为\n",N);
    if(x.sign==-1)
        printf("-");
    fun(z,N);
    printf(".");
    long double bx=0;
    for(int pos=0,i=-1;pos<x.xlen;pos++,i--){
        bx=bx+x.xiaoshu[pos]*pow(10,i);
    }
    for(int i=0;i<MAXSIZE;i++)
    {
        res.xiaoshu[i]=bx*N;
        if(res.xiaoshu[i]<10)
            printf("%d",res.xiaoshu[i]);
        else
            printf("%c",res.xiaoshu[i]-10+'A');
        bx=bx*N-res.xiaoshu[i];
        if(bx==0)
            break;
    }
}
//——————————————————————————————————————————————————————————

int main() {
    dec_num result;             //保存计算结果
    initDecNum(&result);
    dec_num x;                  //输入的十进制数x
    initDecNum(&x);

    int num_terms=0;            //多项式的项数
    printf("请输入多项式的项数：\n");
    scanf("%d",&num_terms);

    printf("请输入自变量x的值：\n");
    decimal_input(&x);          //将控制台数据存入x中

    dec_num coef[num_terms];    //系数数组
    int exp[num_terms];         //幂数组
    for(int t=0;t<num_terms;t++){       //从控制台输入系数和幂，并调用ADT执行计算
        printf("请输入第%d项系数：\n",t+1);
        decimal_input(&coef[t]);
        printf("请输入第%d项的幂：\n",t+1);
        scanf("%d",&exp[t]);
        result=add(result,multiply(coef[t],power(x,exp[t])));
    }

    printf("请输入结果需要精确到的位数：\n");
    scanf("%d",&n);
    debugPrint(result,n);
    int baseN=10;
    printf("请输入结果需要转换的进制：\n");
    scanf("%d",&baseN);
    decTobaseN(result,baseN);          //转换为任意进制
    return 0;
}
