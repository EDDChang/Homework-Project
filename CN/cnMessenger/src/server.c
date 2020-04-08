// {{{ preprocess
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#define PORT "12390"
#define BACKLOG 10
#define MAXUSER 32
#define MAXLINE 100
int open_server();
void login();
void start();
void add_friend();
int server;
void recv_file();
void setoff();
void send_file();
void trash_recv();
char username[MAXLINE + 5];
void sendchat();
int fd;
// }}}
// {{{ main
int main(){
	fprintf(stderr,"server is up\n");
	server = open_server();
	fprintf(stderr,"ready to accept\n");
	fd_set master;
	FD_ZERO(&master);
	FD_SET(server,&master);
	while(1){
		fd_set working;
		memcpy(&working,&master,sizeof(working));
		int ret = select(10,&working,NULL,NULL,0);
		if(FD_ISSET(server,&working)){ // new client
			if(fork() == 0){
				fd = accept(server,NULL,NULL);
				fprintf(stderr,"a new client accepted\n");
				login();
				exit(0);
			}
		}
	}
}
// }}}
// {{{ login or sign up  
void login(){
	char cmd[MAXLINE + 5] = "";
	recv(fd,cmd,MAXLINE,0);
	// {{{ sign up
	if(strcmp(cmd,"/signup") == 0){
		fprintf(stderr,"Someone signing up\n");
		send(fd,"0",1,0);
		while(1){
			char name[MAXLINE + 5] = "";
			recv(fd,name,MAXLINE,0);
			DIR *user_dir = opendir(name);
			if(user_dir){
				fprintf(stderr,"Name already used\n");
				send(fd,"-1",2,0);
				closedir(user_dir);
			}
			else if(errno == ENOENT){
				strcpy(username,name);
				send(fd,"0",1,0);
				if(mkdir(name,0777) < 0){
					fprintf(stderr,"mkdir failed\n");
					exit(1);
				}
				chdir(name);
				char passwd[MAXLINE + 5] = "";
				recv(fd,passwd,MAXLINE,0);
				send(fd,"0",1,0);
				for(int i = 0;i<strlen(passwd);i++){
					passwd[i] += 3;
				}
				FILE *pwd = fopen("pwd","w");
				if(pwd == NULL){
					fprintf(stderr,"pwd can't create\n");
				}
				fprintf(stderr,"%s\n",passwd);
				fprintf(pwd,"%s\n",passwd);
				fprintf(stderr,"Welcome %s\n",name);
				fclose(pwd);
				break;
			}
			else{
				fprintf(stderr,"open dir failed\n");
			}
		}
	}
	// }}}
// {{{ login   
	else if(strcmp(cmd,"/login") == 0){
		fprintf(stderr,"Someone login\n");
		send(fd,"0",1,0);
		while(1){
			char name[MAXLINE + 5] = "";
			recv(fd,name,MAXLINE,0);
			if(strlen(name) == 0){
				exit(0);
			}
			DIR *check_dir = opendir(name);
			if(check_dir){
				if(chdir(name) < 0){
					fprintf(stderr,"chdir failed\n");
					exit(1);
				}
				send(fd,"0",1,0);
				strcpy(username,name);
				FILE *fp = fopen("pwd","r");
				if(fp == NULL){
					fprintf(stderr,"pwd not found\n");
					exit(1);
				}
				char currectpwd[MAXLINE + 5] = "";
				fscanf(fp,"%s",currectpwd);
				while(1){
					char passwd[MAXLINE + 5] = "";
					recv(fd,passwd,MAXLINE,0);
					for(int i = 0;i<strlen(passwd);i++){
						passwd[i] += 3;
					}
					if(strcmp(passwd,currectpwd) == 0){
						send(fd,"0",1,0);
						fprintf(stderr,"Welcome %s\n",name);
						break;
					}
					else send(fd,"-1",2,0);
				}
				fclose(fp);
				break;
			}
			else if(errno == ENOENT){
				send(fd,"-1",2,0);
				fprintf(stderr,"Name not found\n");
			}
			else{
				fprintf(stderr,"open dir failed\n");
				exit(1);
			}
		}
	}
// }}}
	else{
		send(fd,"-1",2,0);
		fprintf(stderr,"cmd wrong\n");
		exit(1);
	}
	FILE *friend = fopen("friend","r");
	if(friend != NULL){
		char fname[MAXLINE + 5] = "";
		while(fscanf(friend,"%s",fname) != EOF){
			send(fd,fname,strlen(fname),0);
			fprintf(stderr,"send %s\n",fname);
			char tmp[2];
			recv(fd,tmp,2,0);
		}
		fclose(friend);
	}
	send(fd,"-1",2,0);
	fprintf(stderr,"send friend done\n");
	FILE *status = fopen("status","w");
	if(status == NULL){
		fprintf(stderr,"open failed\n");
		exit(1);
	}
	fprintf(status,"on");
	fclose(status);
	start();
}
// }}}
// {{{ recvdata
void recvdata(){
	char name[MAXLINE + 5] = "";
	recv(fd,name,MAXLINE,0);
	send(fd,"0",1,0);
	trash_recv();
	strcat(name,"log");
	int off = 0;
	while(1){
		FILE *fp = fopen(name,"r");
		fseek(fp,off,SEEK_SET);
		char buf[MAXLINE + 5] = "";
		fgets(buf,MAXLINE,fp);
		int newoff = ftell(fp);
		fclose(fp);
		if(newoff == off) {
			fd_set master;
			FD_ZERO(&master);
			FD_SET(fd,&master);
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			int ret = select(32,&master,NULL,NULL,&tv);
			if(ret == 0) continue;
			if(FD_ISSET(fd,&master)){
				char t[10];
				recv(fd,t,5,0);
				if(strcmp(t,"-1") == 0){
					send(fd,"0",1,0);
					break;
				}
			}
			continue;
		}
		off = newoff;
		fprintf(stderr,"buf = %s\n",buf);
		send(fd,buf,strlen(buf),0);
		char tmp[10] = "";
		recv(fd,tmp,5,0);
		if(strcmp(tmp,"-1") == 0){
			send(fd,"0",1,0);
			break;
		}
	}
	
}
// }}}
// {{{ main menu  
void start(){
	fd_set master;
	FD_ZERO(&master);
	FD_SET(fd,&master);
	while(1){
		char cmd[MAXLINE + 5] = "";
		fd_set working;
		memcpy(&working,&master,sizeof(working));
		int ret = select(10,&working,NULL,NULL,0);
		if(ret == 0) continue;
		recv(fd,cmd,MAXLINE,0);
		if(strcmp(cmd,"add") == 0){
			fprintf(stderr,"add friend\n");
			send(fd,"0",1,0);
			add_friend();
		}
		else if(strcmp(cmd,"sendchat") == 0){
			send(fd,"0",1,0);
			fprintf(stderr,"send chat\n");
			sendchat();
		}
		else if(strcmp(cmd,"recvchat") == 0){
			send(fd,"0",1,0);
			fprintf(stderr,"recv chat\n");
			recvdata();
		}
		else if(strcmp(cmd,"send") == 0){
			send(fd,"0",1,0);
			fprintf(stderr,"receive file\n");
			recv_file();
		}
		else if(strcmp(cmd,"recv") == 0){
			send(fd,"0",1,0);
			fprintf(stderr,"send file to User\n");
			send_file();
		}
		else if(strcmp(cmd,"logout") == 0){
			fprintf(stderr,"logout\n");
			setoff();
			exit(0);
		}
	}
}
// }}}
// {{{ send chat
void sendchat(){
	char name[MAXLINE + 5] = "";
	recv(fd,name,MAXLINE,0);
	send(fd,"0",1,0);
	char p2[MAXLINE + 5] = "";
	strcpy(p2,name);
	strcat(p2,"log");
	FILE *log = fopen(p2,"a");
	char path[MAXLINE + 5] = "../";
	strcat(path,name);
	strcat(path,"/");
	strcat(path,username);
	strcat(path,"log");
	FILE *fp = fopen(path,"a");
	fprintf(stderr,"path = %s\n",path);
	while(1){
		char buf[MAXLINE + 5] = "";
		int s = recv(fd,buf,MAXLINE,0);
		fprintf(stderr,"%s",buf);
		send(fd,"0",1,0);
		if(strcmp(buf,"end\n") == 0) break;
		fprintf(log,"%s: %s",username,buf);
		fprintf(fp,"%s: %s",username,buf);
		fflush(log);
		fflush(fp);
	}
}
// }}}
// {{{ recv_file 
void recv_file(){
	char name[MAXLINE + 5] = "";
	recv(fd,name,MAXLINE,0);
	fprintf(stderr,"send to %s\n",name);
	char path[MAXLINE + 5] = "../";
	strcat(path,name);
	strcat(path,"/status");
	FILE *status = fopen(path,"r");
	char code[10];
	fscanf(status,"%s",code);
	if(strcmp(code,"on") == 0){
		send(fd,"0",1,0);
		int file_num;
		recv(fd,&file_num,4,0);
		fprintf(stderr,"send %d files\n",file_num);
		send(fd,"0",1,0);
		for(int i = 0;i<file_num;i++){
			char p[MAXLINE + 5] = "../";
			strcat(p,name);
			strcat(p,"/");
			strcat(p,username);
			strcat(p,"/");
			char filename[MAXLINE + 5] = "";
			recv(fd,filename,MAXLINE,0);
			strcat(p,filename);
			FILE *fp = fopen(p,"w");
			send(fd,"0",1,0);
			int filesize;
			recv(fd,&filesize,4,0);
			fprintf(stderr,"filesize = %d\n",filesize);
			send(fd,"0",1,0);
			char buf[65535] = "";
			int cur = 0;
			while(cur < filesize ){
				int b = recv(fd,buf,sizeof(buf),0);
				fprintf(stderr,"b = %d\n",b);
				cur += b;
				int r = fwrite(buf,1,b,fp);
				fflush(fp);
				fprintf(stderr,"r = %d\n",r);
				send(fd,"0",1,0);
			}
			char tmp[10] = "";
			recv(fd,tmp,10,0);
			if(strcmp(tmp,"done") == 0){
				send(fd,"0",1,0);
				fprintf(stderr,"Done\n");
			}
			else{
				fprintf(stderr,"Something is wrong\n");
				setoff();
				exit(1);
			}

		}
	}
	else{
		send(fd,"-1",2,0);
		fprintf(stderr,"User offline\n");
	}
	return;
	
}
// }}}
// {{{ add_friend
void add_friend(){
	char name[MAXLINE + 5] = "";
	recv(fd,name,MAXLINE,0);
	if(strcmp(name,"-1") == 0 || strlen(name) == 0) return;
	char path[MAXLINE + 5] = "../";
	strcat(path,name);
	fprintf(stderr,"path = %s\n",path);
	DIR *chk = opendir(path);
	if(chk){
		closedir(chk);
		fprintf(stderr,"This user exist\n");
		send(fd,"0",1,0);
		FILE *fp = fopen("friend","a");
		if(fp == NULL){
			fprintf(stderr,"fopen failed\n");
			setoff();
			exit(1);
		}
		fprintf(fp,"%s\n",name);
		char path2[MAXLINE + 5] = "";
		strcpy(path2,path);
		strcat(path,"/friend");
		FILE *fp2 = fopen(path,"a");
		if(fp == NULL){
			fprintf(stderr,"fopen failed\n");
			setoff();
			exit(1);
		}
		fprintf(fp2,"%s\n",username);
		fclose(fp2);
		fclose(fp);
		mkdir(name,0777);
		strcat(path2,"/");
		strcat(path2,username);
		fprintf(stderr,"path2 = %s\n",path2);
		mkdir(path2,0777);
		strcat(name,"log");
		fp = fopen(name,"w");
		fclose(fp);
		strcat(path2,"log");
		fp = fopen(path2,"w");
		fclose(fp);
	}
	else if(errno == ENOENT){
		fprintf(stderr,"User not found\n");
		send(fd,"-1",2,0);
	}
	else{
		fprintf(stderr,"open dir failed\n");
		setoff();
		exit(1);
	}
	return;
}
// }}}
// {{{ set off
void setoff(){
	FILE *fp = fopen("status","w");
	fprintf(fp,"off");
	return;
}
// }}}
// {{{send file
void send_file(){
	char name[MAXLINE + 5] = "";
	recv(fd,name,MAXLINE,0);
	int num = 0;
	struct dirent *d;	
	DIR *dir = opendir(name);
	if(dir == NULL){
		fprintf(stderr,"Open dir wrong\n");
		exit(1);
	}
	while((d = readdir(dir)) != NULL) num++;
	if(num == 2) send(fd,"-1",2,0);
	else send(fd,"0",1,0);
	trash_recv();
	num-=2;
	send(fd,&num,4,0);
	trash_recv();
	closedir(dir);
	dir = opendir(name);
	fprintf(stderr,"%d files to transfer\n",num);
	while((d = readdir(dir)) != NULL){
		if(strcmp(d->d_name,".") != 0 && strcmp(d->d_name,"..") != 0){
			char fname[MAXLINE + 5] = "";
			strcpy(fname,d->d_name);
			char path[MAXLINE + 5] = "";
			strcpy(path,name);
			strcat(path,"/");
			strcat(path,d->d_name);
			FILE *fp = fopen(path,"r");
			fseek(fp,0,SEEK_END);
			int size = ftell(fp);
			fseek(fp,0,SEEK_SET);
			fprintf(stderr,"size = %d\n",size);
			send(fd,&size,4,0);
			trash_recv();
			int tmp = send(fd,fname,strlen(fname),0);
			fprintf(stderr,"tmp = %d\n",tmp);
			fprintf(stderr,"Start\n");
			trash_recv();
			char buf[65535] = "";
			int cur = 0;
			while(cur < size){
				int s = fread(buf,1,sizeof(buf),fp);
				fprintf(stderr,"s = %d\n",s);
				int a = send(fd,buf,s,0);
				fprintf(stderr,"a = %d\n",a);
				cur += s;
				trash_recv();
			}
			send(fd,"done",4,0);
			trash_recv();
			fprintf(stderr,"Done\n");
			remove(path);
			fclose(fp);
			
		}
	}
}
// }}}
// {{{ trash_recv
void trash_recv(){
	char tmp[20];
	recv(fd,tmp,10,0);
}
// }}}
// {{{ open server
int open_server(){
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status = getaddrinfo(NULL,PORT,&hints,&servinfo)) != 0){
		fprintf(stderr,"getaddrinfo error: %s\n",gai_strerror(status));
		exit(EXIT_FAILURE);
	}
	int server_fd;
	if((server_fd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) < 0){
		fprintf(stderr,"socket error\n");
		exit(EXIT_FAILURE);
	}
	
	if(bind(server_fd,servinfo->ai_addr,servinfo->ai_addrlen) < 0){
		fprintf(stderr,"bind error = %d\n",errno);
		exit(EXIT_FAILURE);
	}
	
	if(listen(server_fd,BACKLOG) < 0){
		fprintf(stderr,"listen error\n");
		exit(EXIT_FAILURE);
	}
	return server_fd;
}
// }}}
