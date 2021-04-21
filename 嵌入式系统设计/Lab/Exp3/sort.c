//字符串比较函数，若Str1>Str2,返回正数
int strcmp(char* str1, char* str2) {
    int i = 0;
    while (str1[i] == str2[i] && str1[i] != 0)
        i++;
    return str1[i] - str2[i];
}
//字符串排序函数
void strsort(char** strs, int n) {
    char *tmp; //地址的暂存器
    //冒泡排序
    for(int i=0;i<n;i++)
        for(int j=i+1;j<n;j++){
            //若strs[i]内地址所指的字符串大于strs[j]内地址所指的字符串，则交换这两个地址值
            if(strcmp(strs[i], strs[j]) > 0) {
                tmp = strs[i];
                strs[i] = strs[j];
                strs[j]=tmp;
            }
        }
}