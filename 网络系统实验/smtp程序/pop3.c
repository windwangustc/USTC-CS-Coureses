#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>

//邮件主题、正文、附件
/*
char Subject[] ="";
char MainBody[] ="";
char Attachment[] ="";
*/
#define USER          "USER windwang@mail.ustc.edu.cn\r\n"        /* 邮箱账号 */
#define PASS          "PASS xxxxxxxxx\r\n"                        /* 邮箱密码or授权码 */
#define RETR1         "retr 1\r\n"                               /*获取邮件*/
#define QUIT          "QUIT\r\n"                                /*断开连接*/

#if 1
void ShowMailbyPOP3(const char* popserver,const short nPort){
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *host;
    /*取得主机IP地址*/
    if((host=gethostbyname(popserver))==NULL){
        fprintf(stderr,"Get hostname error, %s\n", strerror(errno));
        exit(1);
    }
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*建立SOCKET连接*/{
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        exit(1);
    }
    /* 客户程序填充服务端的资料 */
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(nPort);
    server_addr.sin_addr=*((struct in_addr *)host->h_addr);

/* 客户程序发起连接请求 */
/*连接邮件服务器*/
    printf("Client Connecting to Server: %s on port: %d\n",popserver, nPort);
    if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1){
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
        exit(1);
    }

    char szBuf[1024*8];
    memset(szBuf,0,1024);

    recv(sockfd,szBuf,sizeof(szBuf)+1,0);
    printf("S: %s", szBuf);

//发送准备登陆信息
    send(sockfd, USER, strlen(USER), 0);
    printf("C: USER windwang@mail.ustc.edu.cn\n");
    recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);

    send(sockfd, PASS, strlen(PASS), 0);
    recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);
//获取未读邮件
    memset(szBuf,0,sizeof(szBuf));
    send(sockfd, RETR1, strlen(RETR1), 0);
    recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);
    //断开链接
    memset(szBuf,0,sizeof(szBuf));
    send(sockfd, QUIT, strlen(QUIT), 0);
    recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);
    close(sockfd);
}
#endif

int main(int argc, char **argv)
{
    char popserver[]="mail.ustc.edu.cn";
    short nPort=110;     //POP3协议端口号为110
    ShowMailbyPOP3(popserver,nPort);
    return 0;
}