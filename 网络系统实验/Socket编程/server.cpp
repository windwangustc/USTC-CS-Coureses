#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>

#define BUFLEN 1024
#define PORT 6665
#define LISTNUM 20

int server(){
    int sockfd,newfd;       //socket handler
    struct  sockaddr_in s_addr,c_addr;  //for server and client address
    char buf[BUFLEN];       //聊天数据流
    socklen_t len;          //accept()调用参数，socklen_t长度和int相同
    unsigned int port,listnum;//端口号，连接请求暂存队列长度
    fd_set rfds;            //文件描述字结构体，与文件句柄(socket、文件、管道、设备等)建立联系
    struct timeval tv;      //超时时间
    int retval,maxfd;

    //establish socket and get a handler
    //check socket() for more info about arguments
    if((sockfd=socket(PF_INET,SOCK_STREAM,0))==-1){     //tcp/ip protocol，顺序、可靠、双向数据流，TCP 连接。
        perror("socket");
        exit(errno);
    }else
        printf("Socket create sucess!\n");
    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(PORT);
    s_addr.sin_addr.s_addr=htons(INADDR_ANY);

    //bind addr and port procedure
    if((bind(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr))==-1)){
        perror("bind");
        exit(errno);
    }else
        printf("bind success!\n");
    /*侦听本地端口*/
    if(listen(sockfd,listnum) == -1){
        perror("listen");
        exit(errno);
    }else
        printf("the server is listening!\n");

    //Communication part
    while (1){
        printf("**********************聊天开始**********************\n");
        len =sizeof (struct sockaddr);
        if((newfd=accept(sockfd,(struct sockaddr*)&c_addr,&len))==-1 ){
            perror("accept");
            exit(errno);
        }else
            printf("正在与您聊天的客户端是：%s: %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
        while (1){
            FD_ZERO(&rfds);
            FD_SET(0,&rfds);
            maxfd=0;
            FD_SET(newfd,&rfds);
            //找出文件描述符集合中最大的文件描述符
            if(maxfd<newfd)
                maxfd=newfd;
            //设置超时时间
            tv.tv_sec=6;
            tv.tv_usec=0;
            //等待聊天
            retval=select(maxfd+1,&rfds,NULL,NULL,&tv);
            if(retval==-1){
                printf("select出错，与该客户端连接的程序将退出\n");
                break;
            } else if(retval==0) {
                printf("waiting......\n");
                continue;
            }else{
                //用户输入信息
                if(FD_ISSET(0,&rfds)) {
                    //发送消息
                    memset(buf,0,sizeof(buf));
                    //fgets函数：从流中读取BUFLEN-1个字符
                    fgets(buf,BUFLEN,stdin);
                    //打印发送的消息
                    if(!strncasecmp(buf,"quit",4)){
                        printf("server请求终止聊天!\n");
                        break;
                    }else
                        len = send(newfd,buf,strlen(buf),0);
                    if(len>0)
                        printf("\t消息发送成功：%s\n",buf);
                    else{
                        printf("消息发送失败！\n");
                        break;
                    }
                }
            }

            //客户端发来消息
            if(FD_ISSET(newfd,&rfds)){
                //接收消息
                memset(buf,0,sizeof(buf));
                //fgets函数：从流中读取BUFLEN-1个字符
                //fgets(buf,BUFLEN,stdin);
                len=recv(newfd,buf,BUFLEN,0);
                if(len > 0)
                    printf("客户端发来的信息是：%s\n",buf);
                else {
                    if (len < 0)
                        printf("接受消息失败！\n");
                    else
                        printf("客户端退出了，聊天终止！\n");
                    break;
                }
            }
        }
        //关闭聊天的套接字
        close(newfd);
        //是否推出服务器
        printf("服务器是否退出程序：y->是；n->否? ");
        bzero(buf, BUFLEN);
        fgets(buf,BUFLEN, stdin);
        if(!strncasecmp(buf,"y",1)){
            printf("server 退出!\n");
            break;
        }
    }
    //关闭套接字
    close(sockfd);
    return 0;
}
int main() {
    server();
    return 0;
}
