#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
int main(int argc,char *argv[])
{
    if(argc<2)
    {
        printf("eg: ./a.out port\n");
        exit(1);
    }
    int port=atoi(argv[1]);
    //1、创建socket
    int fd=socket(AF_INET,SOCK_STREAM,0);
    //2、connect 连接服务器
    struct sockaddr_in    server;
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    inet_pton(AF_INET,"127.0.0.1",&server.sin_addr.s_addr);
    connect(fd,(struct sockaddr*)&server,sizeof(server));
    
    //3、通信
    while(1)
    {
        //发送数据
        char buf[1024];
        printf("请输入要发送的字符串\n");
        fgets(buf,sizeof(buf),stdin);
        write(fd,buf,strlen(buf));

        //等待接收数据
        int len=read(fd,buf,sizeof(buf));
        if(len==-1)
        {
            perror("read error");
            exit(1);
        }
        else if(len==0)
        {
            printf( "服务器关闭了连接\n");
            break;
        }
        else
        {
            printf("recv buf:%s\n ",buf);
        }
    }
    //4、关闭文件描述符
    close(fd);
    printf("Hello world\n");
    return 0;
}

