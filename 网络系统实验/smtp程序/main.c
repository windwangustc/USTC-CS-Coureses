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
char Subject[] ="From: 455933934@qq.com\r\n"
                 "To: windwang@mail.ustc.edu.cn\r\n"
                 "Subject: test\r\n"
                 "MIME-Version: 1.0\r\n"
                 "Content-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n\r\n";

char MainBody[] ="--#BOUNDARY#\r\n"
                 "Content-Type: text/plain; charset=utf-8\r\n"
                 "Content-Transfer-Encoding: quoted-printable\r\n\r\n"
                 "PE20060014 王晨 网络系统实验";

char Attachment[] ="\r\n--#BOUNDARY#\r\n"
                 "Content-Type: application/octet-stream; name=attachment.txt\r\n"
                 "Content-Disposition: attachment; filename=attachment.txt\r\n"
                 "Content-Transfer-Encoding: base64\r\n\r\n"
                 "UEUyMDA2MDAxNCDnjovmmagg572R57uc57O757uf5a6e6aqM";

void StreamClient(char *szServer, short nPort) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *host;
    /*取得主机IP地址*/
    if((host=gethostbyname(szServer))==NULL){
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
    printf("Client Connecting to Server: %s on port: %d\n",szServer, nPort);
    if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1){
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
        exit(1);
    }

    char szBuf[1024];
    memset(szBuf,0,1024);
    int nRet;
    char buf[350]="0";
    char server[250];
    gethostname(server,250);
    strcpy(buf, "HELO ");
    strcat(buf, szServer);
    strcat(buf, " \r\n");
    //printf("C: %s",buf);

//
// Wait for a reply
//
    nRet = recv(sockfd,szBuf,sizeof(szBuf)+1,0);
    printf("S: %s", szBuf);
    nRet = send(sockfd, buf, strlen(buf), 0);
    printf("C: %s",buf);
    nRet = recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);

//发送准备登陆信息
    nRet = send(sockfd, "AUTH LOGIN\r\n", strlen("AUTH LOGIN\r\n"), 0);
    printf("C: AUTH LOGIN\n");
    nRet = recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);

//发送用户名和密码（授权码）,这里的用户名和密码（授权码）必须用base64进行转码,发送转码以后的字符串，对于qq邮箱来说用户名是@前面的字符串
    nRet = send(sockfd,"NDU1OTMzOTM0\r\n", strlen("NDU1OTMzOTM0\r\n"),0);
    printf("C: Sending USERNAME\n");
    nRet = recv(sockfd, szBuf, sizeof(szBuf),0);
    printf("S: %s", szBuf);

//发送用户密码,注意如果是qq邮箱，这里需要发送授权码
    nRet = send(sockfd,"emh5Y2FtaHhrZ3VzYmpkZQ==\r\n", strlen("emh5Y2FtaHhrZ3VzYmpkZQ==\r\n"), 0);
    printf("C: Sending PASSWORD\n");
    nRet = recv(sockfd, szBuf, sizeof(szBuf),0);
    printf("S: %s", szBuf);

//发送[发送邮件]的信箱，该邮箱要与用户名一致，否则发送不成功
    send(sockfd,"MAIL FROM: <455933934@qq.com>\r\n",strlen("MAIL FROM: <455933934@qq.com>\r\n"),0);
    printf("C: MAIL FROM: <455933934@qq.com>\n");
    memset(szBuf,0,sizeof(szBuf));
    nRet = recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);

//发送[接收邮件]的邮箱
    nRet= send(sockfd,"RCPT TO: <windwang@mail.ustc.edu.cn>\r\n",strlen("RCPT TO: <windwang@mail.ustc.edu.cn>\r\n"),0);
    printf("C: RCPT TO: <windwang@mail.ustc.edu.cn>\n");
    memset(szBuf,0,sizeof(szBuf));
    nRet = recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);


//各诉邮件服务器，准备发送邮件内容
    send(sockfd,"DATA\r\n", strlen("DATA\r\n"),0);
    printf("C: DATA\n");
    memset(szBuf,0,sizeof(szBuf));
    nRet = recv(sockfd, szBuf, sizeof(szBuf), 0);
    printf("S: %s", szBuf);
//发送邮件标题、内容、附件
    printf("C: Sending email now\n");
    send(sockfd,Subject, strlen(Subject),0);
    send(sockfd,MainBody, strlen(MainBody),0);
    send(sockfd,Attachment, strlen(Attachment),0);
    //send(sockfd,"PE20060014 王晨 网络系统实验\r\n", strlen("PE20060014 王晨 网络系统实验\r\n"),0);
//发送邮件结束
    send(sockfd,"\r\n.\r\n", strlen("\r\n.\r\n"),0);
//接收邮件服务器返回信息
    memset(szBuf,0,sizeof(szBuf));
    nRet = recv(sockfd,szBuf,sizeof(szBuf),0);
    printf("S: %s", szBuf);
    send(sockfd,"QUIT\r\n", strlen("QUIT\r\n"),0);
    printf("C: QUIT\n");
    memset(szBuf,0,sizeof(szBuf));
    nRet = recv(sockfd,szBuf,sizeof(szBuf),0);
    printf("S: %s", szBuf);
    close(sockfd);
}

int main(int argc, char **argv)
{
    char hostname[255];
    int nRet;
    short nPort=25;     //smtp协议端口号为25
    StreamClient("smtp.qq.com", nPort);
    //ShowMailbyPOP3();

    return 0;
}
