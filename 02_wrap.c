#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include"02_wrap.h"
#include<errno.h>
#include<sys/types.h>
       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>

void perr_exit(const char *s)
{
    perror(s);
    exit(1);
}
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
    int n;
again:
    n=accept(fd,sa,salenptr);

    if(n<0)
    {
        //ECONNABORTED 发生在重传（一定的次数）失败后，强制关闭套接字
        //EINTR进程信号被中断
        if(errno==ECONNABORTED|| (errno==EINTR))
        {
            goto again;
        }
        else
        {
            perr_exit("accept error");
        }

    }
    return n;

}
int Bind(int fd,const struct sockaddr *sa,socklen_t salen)
{
    int n;
    n=bind(fd,sa,salen);
    if(n<0)
    {
        perror("bind error");
    }
    return n;
}
int Connect(int  fd,const struct sockaddr *sa,socklen_t salen)
{
    int n;
    n=connect(fd,sa,salen);
    if(n<0)
    {
        perr_exit("connect error");
    }
    return n;
}
int Listen(int fd,int backlog)
{
    int n;
    n=listen(fd,backlog);
    if(n<0)
    {
        perr_exit("listen error");
    }
    return n;
}
int Socket(int family,int type,int protocal)
{
    int n;
    n=socket(family,type,protocal);
    if(n<0)
    {
        perr_exit("socket error");
    }
    return n;
}

ssize_t Read(int fd,void *ptr,size_t nbytes)
{
    ssize_t n;
again:
    n=read(fd,ptr,nbytes);
    if(n<0)
    {
        if(errno==EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}
ssize_t Write(int fd,const void *ptr,size_t nbytes)
{
    ssize_t n;
again:
    n=write(fd,ptr,nbytes);
    if(n<0)
    {
        if(errno==EINTR)
            goto again;
        else
            return -1;

    }
    return n;
}
int Close(int fd)

{
    int n;
    n=close(fd);
    if(n<0)
    {
        perror("close error");
    }
    return n;
}
//参数三  应该读取的字节数；
ssize_t Readn(int fd,void *vptr, size_t n )
{
    ssize_t nleft;
    ssize_t nread;
    char *ptr;
    ptr=vptr;
    nleft=n;
    while(nleft>0)
    {
        nread=read(fd,ptr,nleft);
        if(nread<0)
        {
            if(errno==EINTR)
                nread=0;
            else
                return -1;
        }
        else if (nread==0)
            break;
        nleft-=nread;
        ptr+=nread;

    }
    return n-nleft;

}

ssize_t Writen(int fd,const void *vptr,size_t n)
{
    ssize_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr=vptr;
    nleft=n;
    while(nleft>0)
    {
        nwritten=write(fd,ptr,nleft);
        if(nwritten<=0)
        {
            if(nwritten<0&&errno==EINTR)
                nwritten=0;
            else
                return -1;
        }
        nleft-=nwritten;
        ptr+=nwritten;
    }
    return n;

}

ssize_t My_read(int fd,char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];
    if(read_cnt<=0)
    {
again:
        if((read_cnt=read(fd,read_buf,sizeof(read_buf)))<0)
        {
            if(errno==EINTR)
                goto again;
            return -1;
        }
        else if(read_cnt==0)
            return 0;
        read_ptr=read_buf;
    }
    read_cnt--;
    *ptr=*read_ptr++;
    return 1;
}
ssize_t Readline(int fd,void *vptr,size_t maxLen)
{
    ssize_t n,rc;
    char c,*ptr;
    ptr=vptr;

    for(n=1;n<maxLen;n++)
    {
        if((rc=My_read(fd,&c))==1)
        {
            *ptr++=c;
            if(c=='\n')
                break;
        }
        else if(rc==0)
        {
            *ptr=0;
            return n-1;
        }
        else 
            return -1;
    }
    *ptr=0;
    return n;
}

