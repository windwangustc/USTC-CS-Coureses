/***Lab06 code Author:PE20060014 Date:13/1/2021***/
/***lab05() was written based on unix C++ library,exception may occur if running this code on Windows***/
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <sys/ioctl.h>
/***--------------Lab02 Greatest Common Divisor--------------***/
//使用辗转相除法，递归实现
int gcd(const int a,const int b){
    if(a%b==0)              // 递归出口
        return b;
    else
        return gcd(b,a%b);
}

void lab02(){
    printf("\n/***--------------Lab02 Greatest Common Divisor--------------***/\n");
    int a,b;
    int GCD;
    do {
        printf("请输入两个待求GCD的正整数：\n");
        scanf("%d", &a);
        scanf("%d", &b);
        if (a<=0||b<=0) printf("非法输入，请输入两个正整数\n");
        else if (a<b){       //保持a>=b
            int tmp;
            tmp=a;
            a=b;
            b=tmp;
        }
    } while(a<=0||b<=0);
    GCD=gcd(a,b);
    printf("%d和%d的最大公因数为：%d\n",a,b,GCD);
}

/***--------------Lab03 The Linked-List Sort--------------***/
//链表排序
#define HEADVALUE 65535
typedef struct Lnode {      //typedef链表节点，其中包含一个数据，和一个指向下个节点的指针
    int data;               //数据域
    struct Lnode *next;     //指针域
}LinkNode, *LinkList;
LinkNode *initList(){       //构造一个链表并为其分配头节点
    LinkNode *p = (LinkNode *)malloc(sizeof(LinkNode)); //初始化
    if (!p){
        printf("初始化分配头节点失败！\n");
        exit(0);
    }
    p->next = NULL;         //初始化(*p).next
    p->data = HEADVALUE;    //设置头结点的数据域
    return p;               //返回值为初始化后的p
}
LinkNode *ListInsert(LinkList p,int i,int e){      //在第i个位置"之前"插入元素e
    if (i<1) return NULL;
    for(;p;p=p->next)
        if (--i<1){     //寻找第i-1位置
            LinkNode *q = (LinkNode*)malloc(sizeof(LinkNode));
            if(!q){
                printf("插入节点时，分配空间失败！\n");
                exit(0);
            }
            q->next = p->next;  //插入操作，q->next指向p->next
            p->next = q;        //p->next指向q
            q->data = e;
            return q;
        }
    return NULL;
}
void ListBubbleSort(LinkNode *first){      //升序排列链表元素
    for(LinkNode *p=first->next;p->next!=NULL;p=p->next) {
        for (LinkNode *q=p->next;q!=NULL;q=q->next) {
            if(p->data>q->data){
                int tmp;
                tmp=p->data;
                p->data=q->data;
                q->data=tmp;
            }
        }
    }
}
void ListTraverse(LinkNode *p){      //遍历打印链表的元素
    while (p=p->next)  {
        printf("节点-(*addr)=value :  ");
        printf("(*%lX)=%d\n",p,p->data);
    }
    printf("\n");
}

void lab03(){
    printf("\n/***--------------Lab03 The Linked-List Sort--------------***/\n");
    int length;
    do {
        printf("请输入链表的长度：\n");
        scanf("%d", &length);
        if(length<=0) printf("非法输入值，请重试！\n");
    }while(length<=0);
    LinkNode *first = initList();
    printf("请依次该链表%d个结点的值：\n",length);
    for(int i = 0;i < length;i++){          //采用头插法依次将输入的值插入链表
        int tmpelem;
        scanf("%d", &tmpelem);
        ListInsert(first,1,tmpelem);
    }
    printf("升序排列后的链表为：\n",length);
    ListBubbleSort(first);                  //冒泡排序
    ListTraverse(first);                    //遍历输出链表
    free(first);
}

/***--------------Lab04 The Game of Nim--------------***/
int RowA=3,RowB=5,RowC=8;                     //全局变量 保存当前某一行还剩下的rock数量
int Player=1;                                 //当前的Player，1=Player1，2=Player2

void printPlayer(){                           //如果支持，最好用inline
    printf("Player %d, choose a row and number of rocks:",Player);
}

void printBoard(int numA,int numB,int numC){
    printf("\n");
    printf("ROW A: ");
    while(0!=numA--)
        printf("o");
    printf("\n");
    printf("ROW B: ");
    while(0!=numB--)
        printf("o");
    printf("\n");
    printf("ROW C: ");
    while(0!=numC--)
        printf("o");
    printf("\n");
}
//check input and update if it's legal
 int checkInput(char row,int num){
    switch (row) {
        case 'A':
            if(num>0&&num<=RowA){           //if legal return 0,else return 1
                RowA-=num;
                Player=Player%2+1;          //change Player 1 <-> 2 by MOD
                return 0;
            }
            else
                printf("Invalid move. Try again.\nPlayer %d, choose a row and number of rocks:",Player);
            break;
        case 'B':
            if(num>0&&num<=RowB) {
                RowB -= num;
                Player = Player % 2 + 1;
                return 0;
            }
            else
                printf("Invalid move. Try again.\nPlayer %d, choose a row and number of rocks:",Player);
            break;
        case 'C':
            if(num>0&&num<=RowC){
                RowC-=num;
                Player = Player % 2 + 1;
                return 0;
            }
            else
                printf("Invalid move. Try again.\nPlayer %d, choose a row and number of rocks:",Player);
            break;
        default:
                printf("Invalid move. Try again.\nPlayer %d, choose a row and number of rocks:",Player);
            break;
    }
     return 1;
}

void lab04(){
    printf("\n/***--------------Lab04 The Game of Nim--------------***/\n");
    printf("/***Please press <ENTER> when you finish input Row and Num***/ \n");
    do{
        char input_Row = '#';         //每次更换玩家必须重置上一玩家输入的行号和数字
        int  input_Num = 0;
        printBoard(RowA,RowB,RowC);
        printPlayer();
        do {
            input_Row = getchar();                    //use getchar() to obtain only one single character
            scanf("%d", &input_Num);
            fflush(stdin);                            //清空缓冲区
        }while(1==checkInput(input_Row,input_Num));   //输入非法值，则返回重新输入
    } while (0!=RowA || 0!=RowB || 0!=RowC);
    if(Player==1)
        printf("\nPlayer 1 Wins.\n");
    else
        printf("\nPlayer 2 Wins.\n");
}

/***--------------Lab05 Interrupt a Running Program--------------***/
/*** Attention: lab05() was written based on unix C++,
 *   it may not be able to compile on Windows                   ***/
void delay(long int delay_t)
{
    clock_t start_time; //the start time
    start_time=clock();
    while((clock()-start_time) <delay_t);
}
//键盘响应子函数，MacOS不支持conio.h，需要自己实现
int _kbhit(){
    static const int STDIN = 0;
    static bool initialized = false;
    if (! initialized){
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void lab05() {
    printf("\n/***--------------Lab05 Interrupt a Running Program--------------***/\n");
    printf("/***-------------Please press <ENTER> when you finish input------------***/ \n");
    printf("/***-------------Please press <q> to quit loop-------------***/ \n");
    char ch;
    do {
        printf("ICS2020 ICS2020 ICS2020 ICS2020 ICS2020 \n");
        fflush(stdin);
        delay(1000 * 1000);
        if (_kbhit()) {
            fflush(stdin);
            ch = getchar();
            if (ch >= 48 && ch <= 57) {
                printf("\n%c is a decimal number.\n", ch);
            }
            else if(ch!=10&&ch!='q'){            //忽略回车
                printf("\n%c is not a decimal number.\n", ch);
            }
            delay(500 * 1000);
        }
    }while (ch != 'q');
    printf("\n-------------quit loop-------------\n");
}

int main() {
    //lab02();
    //lab03();
    //lab04();
    lab05();
    return 0;
}
