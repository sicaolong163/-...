#include<errno.h>
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<ctype.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>
//回调函数
void recycle(int num)
{
    pid_t wpid;
    while((wpid=waitpid(-1,NULL,WNOHANG))>0)
    {
        printf("child died ,pid=%d,\n",wpid);
    }

}
int main(int argc,char *argv[])
{
    if(argc<2)
    {
        perror("eg:...../a.out\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    socklen_t serv_len=sizeof(serv_addr);
    int port=atoi(argv[1]);
    // 1、创建套接字
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    //初始化 服务器，sockaddr_in;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;//地址族
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//监听本机所有的ip
    serv_addr.sin_port=htons(port);//设置端口
    //2、bind ip和端口
    bind(listenfd,(struct sockaddr*)&serv_addr,serv_len);
    //3、listen设置同时监听的最大个数
    if(listen(listenfd,36)<0)
    {
        perror("listen error....");
    }
    
    printf("start accept....\n");

    //使用信号回收子进程资源
    //
    struct sigaction act;
    act.sa_handler=recycle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCLD,&act,NULL);

    struct sockaddr_in  client_addr;
    socklen_t cli_len=sizeof(client_addr);
   
    
    
    while(1)
    {
        //父进程 接收连接请求； 没有的话 就阻塞
        //accept 阻塞的时候被信号终端，处理信号对应的操作之后 
        //回来之后 不阻塞了直接返回-1，这时候 errno==EINTR
        int connectfd=accept(listenfd,(struct sockaddr*)&client_addr,&cli_len);
        while(connectfd==-1&&errno==EINTR)
        {
            connectfd=accept(listenfd,(struct sockaddr*)&client_addr,&cli_len);
        }
        printf("connect success....\n");
        //创建子进程；
        pid_t pid=fork();
        if(pid==0)
        {
            //.............子进程..........
           //负责通信
           char ip[64];
           while(1)
           {
               //client ip port
               printf("client ip:%s,port:%d\n",
                      inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip,sizeof(ip)),
                      ntohs(client_addr.sin_port));
               char buf[1024];
               int len=read(connectfd,buf,sizeof(buf));
               if(len==-1)
               {
                    perror("read error");
                    exit(1);
               }
               else if(len==0)
               {
                   printf("客户端断开了连接\n");
                   close(connectfd);
                   break;
               }
               else
               {
                   printf("recv buf:%s\n",buf);
                   write(connectfd,buf,len);
               }
           }
           //关闭子进程
           return 0;
           //...................子进程结束.............
        }
        else if(pid>0)
        {

            //父进程  负责连接
            close(connectfd);//关闭
            //回收子进程资源；
            
        }
    }
    //4、accept
    //5、通信
    //6、close
}
