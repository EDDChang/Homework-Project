///{{{include  
#include<bits/stdc++.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <termio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include<fcntl.h>
#include<sys/select.h>
///}}}
///{{{ varibles      
char server_add[20] = "140.112.30.44";
char server_port[7] = "12390";
int serverfd;
struct addrinfo hints , *res;
int socketfd;
char myusername[100];
char SEND[100],RECV[100];
char friends[50][10000];

void clear(){
    for(int i = 0 ; i < 100 ; i++)
        fprintf(stderr,"\n");
}
void mysend(int x){
    memset(SEND,0,sizeof(SEND));
    sprintf(SEND,"%d",x);
    send(serverfd,SEND,strlen(SEND),0);
}
void myrecv(){
    memset(RECV,0,sizeof(RECV));
    if(recv(serverfd,RECV,sizeof(RECV),0) == 0)
        exit(0);
}
 //}}}
 //{{{chat 
 void sendchat(char thefriend[20]){
    char neirong[1000] = "";
    fgets(neirong,1000,stdin);
    while(strcmp(neirong , "end\n") != 0){
        strcpy(neirong,"");
        fprintf(stderr,"What do you wanna say>>");
        fgets(neirong,1000,stdin);
        fprintf(stderr,"got one line %s",neirong);
        send(serverfd,neirong,strlen(neirong),0);
        myrecv();
        if(strcmp(neirong,"end\n") == 0)
            break;

    }
}
void recvchat(char thefriend[]){
    mysend(0);
    fd_set readfds,master;
    FD_ZERO(&readfds);
    FD_ZERO(&master);
    FD_SET(serverfd,&readfds);
    FD_SET(0,&readfds);
    while(1){
        memcpy(&master,&readfds,sizeof(readfds));
        select(32,&master,NULL,NULL,0);
        if(FD_ISSET(0,&master)){
            fprintf(stderr,"aa");
            char yes[30];
            fprintf(stderr,"aa");
            scanf("%s",yes);
            fprintf(stderr,"aa");
            //myrecv();
            fprintf(stderr,"aa");
            mysend(-1);
            fprintf(stderr,"aa");
            myrecv();
            fprintf(stderr,"aa");
            return;
        }
        if(FD_ISSET(serverfd,&master)){
        char neirong[105] = "";
        recv(serverfd,neirong,sizeof(neirong),0);
        mysend(0);
        if(strcmp(neirong,"end") == 0)
            break;
        fprintf(stderr,"%s",neirong);
        }

    }

}
//}}}
//{{{filetransfer
void send_file(char thefriend[20]){
    char filename[20][30];
    int filenum;
    fprintf(stderr,"How many files do you want to transfer:");
    scanf("%d",&filenum);
    for(int k = 0 ; k < filenum ; k ++){
        fprintf(stderr,"enter file name:");
        scanf("%s",filename[k]);
    }
    //char count[2] ;
    //sprintf(count,"%d",filenum);
    send(serverfd,&filenum,sizeof(int),0);
    myrecv();
   for(int i = 0 ; i < filenum ; i++){
      int fd1 = open(filename[i],O_RDONLY);
      FILE *fp = fopen(filename[i],"r");
      fseek(fp,0,SEEK_END);
      int size = ftell(fp);
      //char sizes[10] = "";
      //sprintf(sizes,"%d",size);
      send(serverfd , filename[i],strlen(filename[i]),0);
      myrecv();
      send(serverfd,&size,4,0);
      myrecv();
      char buf[65535] = "";
      int hoemuch;
       while((hoemuch = read(fd1,buf,sizeof(buf))) > 0){
          fprintf(stderr,"%d bytes read\n",hoemuch);
          int s = send(serverfd,buf,hoemuch,0);
          fprintf(stderr,"%d bytes sent\n",s);
          myrecv(); 
      }
      send(serverfd,"done",strlen("done"),0);
      myrecv();
      fclose(fp);
      close(fd1);
   }
}
#define MAXLINE 100
void recv_file(char a[]){
	int file_num;
    mysend(0);
	recv(serverfd,&file_num,4,0);
	fprintf(stderr,"send %d files\n",file_num);
	send(serverfd,"0",1,0);
	for(int i = 0;i<file_num;i++){
        int filesize;
		recv(serverfd,&filesize,4,0);
		fprintf(stderr,"filesize = %d\n",filesize);
		send(serverfd,"0",1,0);
		char filename[MAXLINE + 5] = "";
		recv(serverfd,filename,MAXLINE,0);
		FILE *fp = fopen(filename,"w");
		send(serverfd,"0",1,0);

		char buf[65535] = "";
		int cur = 0;
		while(cur < filesize ){
			int b = recv(serverfd,buf,sizeof(buf),0);
			fprintf(stderr,"b = %d\n",b);
			cur += b;
			int r = fwrite(buf,1,b,fp);
			fflush(fp);
			fprintf(stderr,"r = %d\n",r);
			send(serverfd,"0",1,0);
		}
		char tmp[10] = "";
		recv(serverfd,tmp,10,0);
		if(strcmp(tmp,"done") == 0){
			send(serverfd,"0",1,0);
			fprintf(stderr,"Done\n");
		}

	}
	return;

}
/*
void recv_file(char thefriend[]){
    int num;
    mysend(0);
    recv(serverfd,&num,sizeof(int),0);
    fprintf(stderr,"num = %d",num);
    mysend(0);
    int size;
    for(int i = 0 ; i < num ; i++){
        FILE *fp;
        int filesize;
        char filename[100] = "";
        recv(serverfd,&filesize,sizeof(int),0);
        fprintf(stderr,"filesize = %d",filesize);
        mysend(0);
        int recvsize = recv(serverfd,filename,sizeof(filename),0);
        fprintf(stderr,"file = %d",recvsize);
        mysend(0);

        fp = fopen(filename,"w");
        int currentsize = 0;
        while(filesize > currentsize){
           // fread(fp,sizeof(buf),1,SEEK_SET)
           //strcpy(buf,"");
            char buf[65535] = "" ;
           int size = recv(serverfd,buf,sizeof(buf),0);
           currentsize += size;
           //fprintf(stderr,"%s\n",buf);
           //fprintf(stderr,"size = %d",size);
           mysend(0);
           fwrite(buf,1,size,fp); 
           
        //   else break;           
        }
        myrecv();
        fprintf(stderr,"recv = %s\n",RECV);
        mysend(0);
        fclose(fp);
    }

}*/
//}}}
//{{{process    
void main_page(){
    //sleep(1);
    //clear();
    fprintf(stderr,"(A)Add friends\n");
    fprintf(stderr,"(C)Chat\n");
    fprintf(stderr,"(L)Logout\n");
    fprintf(stderr,"(S)Send File\n");
    fprintf(stderr,"(R)Recv File\n");
    fprintf(stderr,"------------------------------\n");
}
void process(){
    fprintf(stderr,"Hi %s\n",myusername);
    for(int i = 0 ; i < 5 ; i++){
        fprintf(stderr,"..");
        usleep(200000);
    }
    fprintf(stderr,"\n");
    int friendnum = 0;
    for(friendnum = 0 ; strcmp(friends[friendnum],"-1") != 0 ; friendnum++){
        recv(serverfd,friends[friendnum],sizeof (friends[friendnum]), 0);
        send(serverfd,"0",1,0);
        fprintf(stderr,"%s\n",friends[friendnum]);
        if(strcmp(friends[friendnum],"-1") == 0)
            break;
    }
    while(1){
        main_page();
        char command[1];
        fprintf(stderr ,"What do you want to do >>");
        scanf("%s",command);
        int flag = 0;
        if(command[0] == 'A'){
            send(serverfd,"add",3,0);
            myrecv();
            char newfriend[30] = "";
            fprintf(stderr,"Who you wanna add as friend>>");
            scanf("%s",newfriend);
            for(int i = 0 ; i < friendnum ; i++){
                //fprintf(stderr,"%s %s\n",friends[i],newfriend);
                if(strcmp(friends[i],newfriend) == 0){
                    fprintf(stderr,"%s is already ur friend\n",newfriend);
                    flag = 1;
                   break; 
                }
            }
            if(strcmp(myusername,newfriend) == 0){
                fprintf(stderr,"You cannot add urself as a friend\n");
                flag = 1;
            }
            if(flag == 1){send(serverfd,"-1",2,0);continue;}
            send(serverfd,newfriend,strlen(newfriend),0);
            myrecv();
            if(strcmp(RECV,"-1") == 0)fprintf(stderr,"No such user.\n");
            if(strcmp(RECV,"0") == 0){fprintf(stderr,"%s is now ur friend.\n",newfriend);strcpy(friends[friendnum],newfriend);friendnum++;}
        }
        else if(command[0] == 'C'){
            fprintf(stderr,"Send or recv chat:");
            char whattodo[10];
            scanf("%s",whattodo);
            int flag = 0;
            char thefriend[20] = "";
            fprintf(stderr,"Who to chat with:");
            scanf("%s",thefriend);
            for(int i = 0 ; i < friendnum ; i++)
                if(strcmp(friends[i],thefriend) == 0){flag = 1;break;}
            if(flag == 0)
                fprintf(stderr,"%s is not ur friend\n",thefriend);
            else if(strcmp(whattodo,"recv") == 0)
            {
                //recv_chat(thefriend);
                send(serverfd,"recvchat",8,0);
                myrecv();
                send(serverfd,thefriend,strlen(thefriend),0);
                myrecv();
                recvchat(thefriend);
            }
            else if(strcmp(whattodo,"send") == 0){
                send(serverfd,"sendchat",8,0);
                myrecv();
                send(serverfd,thefriend,strlen(thefriend),0);
                myrecv();
                sendchat(thefriend);
            }
            else
                continue;
        }
        else if(command[0] == 'S'){
            send(serverfd,"send",4,0);
            myrecv();
            //send(serverfd,myusername,strlen(myusername),0);
            //myrecv();
            int flag = 0;
            char thefriend[20] = "";
            while(flag == 0){
            fprintf(stderr,"Who to transfer to:");
            scanf("%s",thefriend);

            for(int i = 0 ; i < friendnum ; i++)
                if(strcmp(friends[i],thefriend) == 0){flag = 1;break;}
            if(flag == 0 || strcmp(RECV,"-1") == 0)
                fprintf(stderr,"You cannot send file to %s\n",thefriend);
            else{
                send(serverfd,thefriend,strlen(thefriend),0);
                myrecv();
                send_file(thefriend);
            }
            }
        }
            else if(command[0] == 'R'){
                send(serverfd,"recv",4,0);
                myrecv();
                int flag = 0;
                char thefriend[20] = "";
                fprintf(stderr,"Who to receive from:");
                while(flag == 0){
                scanf("%s",thefriend);
                for(int i = 0 ; i < friendnum ; i++)
                    if(strcmp(friends[i],thefriend) == 0){flag = 1;break;}
                if(flag == 0 || strcmp(RECV,"-1") == 0)
                    fprintf(stderr,"You cannot recv file from %s\n",thefriend);
                else {
                    send(serverfd,thefriend,strlen(thefriend),0);
                    myrecv();
                    recv_file(thefriend);
                }
                }
            }

        else if(command[0] == 'L'){
            send(serverfd,"logout",6,0);
            exit(0);
        }
    }
}

//}}}
///{{{login 
void login(){
    fprintf(stderr,"歡迎使用cnMessage\n");
    fprintf(stderr,"是否為新用戶：");
    char ans[4];
    scanf("%s",ans);
    while(1){
        memset(SEND,0,sizeof(SEND));
        if(strcmp(ans,"yes") == 0){
            send(serverfd,"/signup",strlen("/signup"),0);
            myrecv();        
            if(RECV[0] != '0'){
                fprintf(stderr,"Something is wrong");
                exit(1);
            }
            fprintf(stderr,"Create username:");
            while(1){
                scanf("%s",SEND);
                send(serverfd,SEND,strlen(SEND),0);
                myrecv();
                fprintf(stderr,"recv = %s\n",RECV);
                if(RECV[0] == '-' && RECV[1] == '1'){
                    fprintf(stderr,"username used\n");
                    fprintf(stderr,"Create username:");
                }
                else if(RECV[0] == '0'){
                    strcpy(myusername,SEND);
                    fprintf(stderr,"Valid name\n");
                    break;
                }
            }
            fprintf(stderr,"Please enter ur passwd:");
            scanf("%s",SEND);
            send(serverfd,SEND,strlen(SEND),0);
            myrecv();
            if(RECV[0] != '0')
                exit(0);
            process();
            return;
        }else{
            send(serverfd,"/login",strlen("/login"),0);
            myrecv();
            fprintf(stderr,"Please enter ur username :");
            scanf("%s",SEND);
            send(serverfd,SEND,strlen(SEND),0);
            myrecv();
            if(RECV[0] == '-' && RECV[1] == '1'){
                fprintf(stderr,"username no found\n");
                continue;
            }
            else if(RECV[0] == '0'){
                strcpy(myusername,SEND);
                fprintf(stderr,"Please enter ur passwd:");
            }
            while(1){
                scanf("%s",SEND);
                send(serverfd,SEND,strlen(SEND),0);
                myrecv();
                if(RECV[0] == '-' && RECV[1] == '1'){
                    fprintf(stderr,"Wrong passwd\n");
                    fprintf(stderr,"Please enter ur passwd:");
                    continue;
                }
                else if(RECV[0] == '0'){process();
                    return;}
                return;
            }
        }
    }
}
//}}}
//{{{main
int main(){
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(server_add,server_port,&hints,&res) < 0)
        fprintf(stderr,"getaddr error\n");
    if((socketfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) < 0)
        fprintf(stderr,"socket err\n");
    if(connect(socketfd,res->ai_addr,res -> ai_addrlen) < 0)
        fprintf(stderr,"connect err\n");
    else{
        serverfd = socketfd;
        clear();
        //send(socketfd,"123",3,0);
        //myrecv();
        //fprintf(stderr,"rec = %s",RECV);
        login();
    }
}
//}}}
