#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<ctype.h>
#include<string.h>


int main(int argc,char *argv[])
{
    //1、创建监听的套接字
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("socket error");
        exit(1);
    }
    //2、lfd和bending的ip port进行绑定
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(8888);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    int ret=bind(lfd,(struct sockaddr *)&server,sizeof(server));
    if(ret==-1)
    {
        perror("bind error");
        return (1);
    }
    //3、设置监听
    ret=listen(lfd,20);
    if(ret==-1)
    {
        perror("listen error");
        exit(1);
    }
    //4、等待接收连接请求
    struct sockaddr_in client;
    socklen_t len=sizeof(client);
    int cfd=accept(lfd,(struct sockaddr*)&client,&len);
    if(cfd==-1)
    {
        perror("accept error");
        exit(1);
    }
    printf("accept successful....!!!1\n");
    //5、通信
    //一直通信；
    while(1)
    {
        //先接收数据；
        char buf[1024]={0};
        int len=read(cfd,buf,sizeof(buf));
        if(len==-1)
        {
            perror("read error.....");
            exit(1);
        }
        else if(len==1)
        {
            printf("客户端已经断开连接\n");
            close(cfd);
            break;
        }
        else
        {
            printf("recv buf: %s\n",buf);
            //转换 小写--->大写
            for(int i=0;i<len;i++)
            {
                buf[i]=toupper(buf[i]);
            }
            printf("send buf: %s\n",buf);
            write(cfd,buf,len);
        }
    }
    //6、关闭套接字
    close(lfd);
    printf("Hello world\n");
    return 0;
}

