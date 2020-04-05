#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BACKLOG 10
struct client{
    int port;
    char addr[128];
    char buf[128];    
    int remain;
};
struct client clist[1024];
int main(int argc, char** argv){
    //check argc
    if(argc != 2){
        fprintf(stderr,"usage: %s [port].\n", argv[0]);
        exit(1);
    }
    //server info
    int status; 
    struct addrinfo hints;
    struct addrinfo *servinfo;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(status = getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0){
        fprintf(stderr, "getaddrinfo error:%s\n",gai_strerror(status));
        exit(1);
    }
    
    //socket,bind,listen
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    listen(sockfd,BACKLOG);
    
    //
    fd_set sel;
    fd_set tmp;
    struct timeval time;
    time.tv_sec = 5;
    FD_ZERO(&sel);
    FD_ZERO(&tmp);
    FD_SET(sockfd,&sel);
    
    int maxfd = getdtablesize();

    while(1){

        tmp = sel;
        int num_ready = select(maxfd,&tmp,NULL,NULL,&time);
        if(num_ready == 0)  continue;

        for(int i = 0; i < maxfd; i++){
            
            if(FD_ISSET(i,&tmp) && sockfd == i){
                struct sockaddr_in cliaddr;
                int clilen = sizeof(cliaddr);
                
                int con_fd = accept(sockfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
                if(con_fd < 0)  fprintf(stderr,"connection error.\n");

                getpeername(sockfd, (struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
                               
                strcpy(clist[con_fd].addr,inet_ntoa(cliaddr.sin_addr));
                clist[con_fd].port = ntohs(cliaddr.sin_port);
                
                char buf[64]="";
                recv(con_fd, buf, sizeof(buf), 0);
                clist[con_fd].remain = atoi(buf);
                FD_SET(con_fd, &sel);     
            }
            else if(FD_ISSET(i,&tmp)){
                int con_fd = i;
                clist[con_fd].remain--;
                char buf[64] = "test\n";
                int ret = send(con_fd, buf, sizeof(buf), 0);

                char buf_recv[64] = "";
                ret = recv(con_fd, buf_recv, sizeof(buf_recv), 0);
                if(ret < 0) 
                    continue;
                else if(ret > 0)    
                    fprintf(stderr,"recv from %s:%d\n",clist[i].addr,clist[i].port);

                if(clist[i].remain == 0 || ret == 0){
                    close(con_fd);
                    FD_CLR(i, &sel);
                }
            }
        }    
    }
    close(sockfd);
    return 0;
}
