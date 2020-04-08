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
#include <pthread.h>
#include <time.h>
struct con_server{
    char ip[64];
    char port[64];
};
struct con_server *conlist[1024];
int packets_num = 0;
double timeout = 1000;
void *con_func(void* tocon){
    char addr[64]="";
    char port[64]="";
    strcpy(addr,((struct con_server*)tocon)->ip);
    strcpy(port,((struct con_server*)tocon)->port);
    int num = packets_num;
    if(num == 0) num = -1;
    while(num != 0){
        num--;
        struct addrinfo hints,*res;
        int sockfd;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo(addr, port,&hints,&res);
        
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0){
            fprintf(stderr,"timeout when connect to server %s\n",addr);
            sleep(1);
            continue;
        }
        if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0){
            fprintf(stderr,"timeout when connect to server %s\n",addr);
            sleep(1);
            continue;
        }

        const double start = clock();
        char buf[60] = "";
       // strcpy(buf,itoa(packets_num));
        send(sockfd, buf, sizeof(buf), 0);

        int ret;

        char buf_send[60] = "test.\n";
		ret = send(sockfd, buf_send, sizeof(buf_send), 0);
		if (ret < 0) break;

		char buf_recv[60] = "";
		ret = recv(sockfd, buf_recv, sizeof(buf_recv), 0);
		if (ret < 0) break;

		const double end = clock();
		if (end - start > timeout)
            fprintf(stderr, "timeout when connect to %s\n", addr);
		else 
            fprintf(stderr, "recv from %s, RTT = %f msec\n", addr, end - start);
		close(sockfd);

    }
    fprintf(stderr,"\n");
    return 0;
}
int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "usage: %s [host]:[port]\n", argv[0]);
		exit(1);
    }
    int start;
    if(strcmp(argv[1],"-n") == 0){
        packets_num = atoi(argv[2]);
        if(strcmp(argv[3],"-t") == 0){
            timeout = atof(argv[4]);
            start = 5;
        }
        else{
            start = 3;
        }
    }
    else if(strcmp(argv[1],"-t") == 0){
        timeout = atof(argv[2]);
        start = 3;
    }
    else{
        start = 1;
    }
    pthread_t p[1024];
    for(int i = start; i < argc; i++){
        char *delim = (char*)":";
        char *pch;
        int tmp = i-start;
        conlist[tmp] = (struct con_server*)malloc(sizeof(struct con_server));
        pch = strtok(argv[i],delim);
        strcpy(conlist[tmp]->ip, pch);
        pch = strtok(NULL, delim);
        strcpy(conlist[tmp]->port,pch);
        pthread_create(&p[tmp], NULL, con_func, (void*)conlist[tmp]);
    }
    /*for(int i = 0; i < argc - start; i++)
        fprintf(stderr,"hostname = %s, port = %s\n",conlist[i].ip,conlist[i].port);*/
    for(int i = start; i < argc; i++){
        pthread_join(p[i-start], NULL);
    }
    return 0;
}
    






































