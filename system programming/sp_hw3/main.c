#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>
#include<unistd.h>
#include"scheduler.h"
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#define SIGUSR3 SIGWINCH
int main(){
	int P, Q, sig_num;
	int SIG_list[10];
	scanf("%d%d",&P,&Q);
	scanf("%d",&sig_num);
	for(int i = 0; i < sig_num; i++)
		scanf("%d",&SIG_list[i]);
	int pfd[2];
	pipe(pfd);
	int pid = fork();
	if(pid == 0){
		char p[5], q[5];
		sprintf(p,"%d",P);
		sprintf(q,"%d",Q);
		close(pfd[0]);
		dup2(pfd[1],1);
		close(pfd[1]);
		execlp("./hw3","./hw3",p,q,"3","0",NULL);
	}
	else{
		close(pfd[1]);
		dup2(pfd[0],0);
		close(pfd[0]);
	}
	char ACK[300];
	for(int i = 0; i < sig_num; i++){
		//printf("sig = %d\n",SIG_list[i]);
		sleep(5);
		switch(SIG_list[i]){
			case 1:
				kill(pid,SIGUSR1);
				break;
			case 2:
				kill(pid,SIGUSR2);
				break;
			case 3:
				kill(pid,SIGUSR3);
				break;
		}
		scanf("%s",ACK);
		if(strcmp(ACK,"SIG3") == 0){
			char qcontent[10];
			scanf("%s",qcontent);
			for(int j = 0; j < strlen(qcontent); j++){
				printf("%c",qcontent[j]);
				if(j != strlen(qcontent)-1)
					printf(" ");
				else
					printf("\n");
			}
		}
	}
	scanf("%s",ACK);
	printf("%s\n",ACK);
	wait(NULL);
	return 0;
}
