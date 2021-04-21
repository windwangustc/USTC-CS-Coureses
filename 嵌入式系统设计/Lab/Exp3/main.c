#include <stdio.h>
#include <stdlib.h>
#define STR_NUM 5
#define MAX_STRLEN 20
extern void strsort(char* strs[], int n);

int main(){
    char* strs[STR_NUM];
    //从终端输入5个20字符以内的字符串
    for (int i = 0; i < STR_NUM; i++) {
        strs[i] = (char *) malloc(sizeof(char) * MAX_STRLEN);
        scanf("%s", strs[i]);       //str[i]是char型指针，scanf字符串到str[i]内保存到地址起始值
    }
    //调用外部的汇编程序
    strsort(strs, STR_NUM);
    //输出排序后的字符串
    for (int i = 0; i < STR_NUM; i++)
        printf("%s\n", strs[i]);

    return 0;
}

