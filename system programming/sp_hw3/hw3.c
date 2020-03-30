#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>
#include<unistd.h>
#include<signal.h>
#include"scheduler.h"
#define SIGUSR3 SIGWINCH
int idx = 0;
char arr[10000];
FCB_ptr Current, Head;
FCB fcb[4];
jmp_buf SCHEDULER;
int P,Q,Task,release;
int mutex = 0;
int queue[5];
sigset_t pending, sig123, sig000;
void sig_handler1(){
	//fprintf(stderr,"function 1\n");
	printf("SIG1\n");
	fflush(stdout);
	sigemptyset(&sig123);
	sigaddset(&sig123,SIGUSR1);
	sigprocmask(SIG_BLOCK,&sig123,NULL);
	longjmp(SCHEDULER,1);
}
void sig_handler2(){
	mutex = 0;
	//fprintf(stderr,"function 2\n");
	printf("SIG2\n");
	fflush(stdout);
	sigemptyset(&sig123);
	sigaddset(&sig123,SIGUSR2);
	sigprocmask(SIG_BLOCK,&sig123,NULL);
	longjmp(SCHEDULER,1);
}
void sig_handler3(){
	//fprintf(stderr,"function 3\n");
	printf("SIG3\n");
	fflush(stdout);
	sigprocmask(SIG_BLOCK,&sig123,NULL);
	char qqqq[10]="";
	int qqindex = 0;
	for(int i = 1; i <= 4; i++){
		if(queue[i] != 0)
			qqqq[qqindex++] = i+'0';
	}
	printf("%s\n",qqqq);
	fflush(stdout);
	Current = Current->Previous;
	longjmp(SCHEDULER,1);
}
void funct_5(int name){
	int a[10000];
	//fprintf(stderr,"name = %d\n",name);
	FCB_ptr tmp;
	tmp  = &fcb[name-1];
	tmp->Name = name;
	if(name == 1){
		Current = tmp;
		Head = tmp;
	}
	else{
		Current->Next = tmp;
		tmp->Previous = Current;
		Current = Current->Next;
		if(name == 4){
			Current->Next = Head;
			Head->Previous = Current;
		}
	}
	switch(name){
		case 1:
			funct_1(name);
			break;
		case 2:
			funct_2(name);
			break;
		case 3:
			funct_3(name);
			break;
		case 4:
			funct_4(name);
			break;
	}
}
void funct_1(int name){
	int i,j;
	int count = 0;
	if(setjmp(Current->Environment) == 0)
		funct_5(2);
	if(mutex != 0 && mutex != 1){
		queue[1] = 1;
		longjmp(SCHEDULER,1);
	}
	else{
		mutex = 1;
		queue[1] = 0;
	}
	for(i = 0; i < P; i++){
		for(j = 0; j < Q ;j++){
			sleep(1);
			arr[idx++] = '1';
			//fprintf(stderr,"%s\n",arr);
		}
		count++;
		if(Task == 2 && count == (release)){
			if(setjmp(Current->Environment) == 0){
				count = 0;
				mutex = 0;
				longjmp(SCHEDULER,1);
			}
			else{
				if(mutex != 0 && mutex != 1)
					longjmp(SCHEDULER,1);
				else
					mutex = 1;
			}
		}
		if(Task == 3){
			sigpending(&pending);
			if(sigismember(&pending,SIGUSR1)||sigismember(&pending,SIGUSR2)||sigismember(&pending,SIGUSR3)){
				if(setjmp(Current->Environment) == 0)
					sigprocmask(SIG_SETMASK,&sig000,NULL);
                else{
                    if(mutex == 0){
                        mutex = 1;
                        queue[1] = 0;
                    }else if(mutex != 1){
                        queue[1] = 0;
                        longjmp(SCHEDULER,1);
                    }
                }
			} 
		}
	}
	mutex = 0;
	longjmp(SCHEDULER,-2);
}
void funct_2(int name){
	int i,j;
	int count = 0;
	if(setjmp(Current->Environment) == 0)
		funct_5(3);
	if(mutex != 0 && mutex != 2){
		queue[2] = 1;
		longjmp(SCHEDULER,1);
	}
	else{
		mutex = 2;
		queue[2] = 0;
	}
	for(i = 0; i < P; i++){
		for(j = 0; j < Q ;j++){
			sleep(1);
			arr[idx++] = '2';
			//fprintf(stderr,"%d %d\n",i,j);
		}
		count++;
		if(Task == 2 && count == (release)){
			if(setjmp(Current->Environment) == 0){
				count = 0;
				mutex = 0;
				longjmp(SCHEDULER,1);
			}
			else{
				if(mutex != 0 && mutex != 2)
					longjmp(SCHEDULER,1);
				else
					mutex = 2;
			}
		}
		if(Task == 3){
			sigpending(&pending);
			if(sigismember(&pending,SIGUSR1)||sigismember(&pending,SIGUSR2)||sigismember(&pending,SIGUSR3))
				if(setjmp(Current->Environment) == 0){
					sigprocmask(SIG_SETMASK, &sig000, NULL);
				}
                else{
                    if(mutex == 0){
                        mutex = 2;
                        queue[2] = 0;
                    }
                    else if(mutex !=2){
                        queue[2] = 1;
                        longjmp(SCHEDULER,1);
                    }
                }
		}
	}
	mutex = 0;
	longjmp(SCHEDULER,-2);
}
void funct_3(int name){
	int i,j;
	int count = 0;
	if(setjmp(Current->Environment) == 0)
		funct_5(4);
	if(mutex != 0 && mutex != 3){
		queue[3] = 1;
		longjmp(SCHEDULER,1);
	}
	else{
		mutex = 3;
		queue[3] = 0;
	}
	for(i = 0; i < P; i++){
		for(j = 0; j < Q ;j++){
			sleep(1);
			arr[idx++] = '3';
			//fprintf(stderr,"%s\n",arr);
		}
		count++;
		if(Task == 2 && count == (release)){
			if(setjmp(Current->Environment) == 0){
				count = 0;
				mutex = 0;
				longjmp(SCHEDULER,1);
			}
			else{
				if(mutex != 0 && mutex != 3)
					longjmp(SCHEDULER,1);
				else
					mutex = 3;
			}
		}
		if(Task == 3){
			sigpending(&pending);
			if(sigismember(&pending,SIGUSR1)||sigismember(&pending,SIGUSR2)||sigismember(&pending,SIGUSR3))
				if(setjmp(Current->Environment) == 0){
					sigprocmask(SIG_SETMASK, &sig000, NULL);
				}
                else{
                    if(mutex == 0){
                       mutex = 3;
                       queue[3] = 0;
                    }else if(mutex != 3){
                        queue[3] = 1;
                        longjmp(SCHEDULER,1);
                    }
                }
		}
	}
	mutex = 0;
	longjmp(SCHEDULER,-2);
}
void funct_4(int name){
	int i,j;
	int count = 0;
	if(setjmp(Current->Environment) == 0)
		longjmp(SCHEDULER,1);
	if(mutex != 0 && mutex != 4){
		queue[4] = 1;
		longjmp(SCHEDULER,1);
	}
	else{
		mutex = 4;
		queue[4] = 0;
	}
	for(i = 0; i < P; i++){
		for(j = 0; j < Q ;j++){
			sleep(1);
			arr[idx++] = '4';
			//fprintf(stderr,"%s\n",arr);
		}
		count++;
		if(Task == 2 && count == (release)){
			if(setjmp(Current->Environment) == 0){
				count = 0;
				mutex = 0;
				longjmp(SCHEDULER,1);
			}
			else{
				if(mutex != 0 && mutex != 1)
					longjmp(SCHEDULER,1);
				else
					mutex = 4;
			}
		}
		if(Task == 3){
			sigpending(&pending);
			if(sigismember(&pending,SIGUSR1)||sigismember(&pending,SIGUSR2)||sigismember(&pending,SIGUSR3))
				if(setjmp(Current->Environment) == 0){
					sigprocmask(SIG_SETMASK, &sig000, NULL);
				}else{
                    if(mutex == 0){
                        mutex = 4;
                        queue[4] = 0;
                    }
                    else if(mutex != 4){
                        queue[4] = 1;
                        longjmp(SCHEDULER,1);
                    }
                }
		}
	}
	mutex = 0;
	longjmp(SCHEDULER,-2);
}

int main(int argc, char** argv){
	P = atoi(argv[1]);
	Q = atoi(argv[2]);
	Task = atoi(argv[3]);
	release = atoi(argv[4]);
	
	struct sigaction act1, act2, act3;
	
	act1.sa_flags = 0;
	act1.sa_handler = sig_handler1;	
	sigaddset(&act1.sa_mask,SIGUSR2);
	sigaddset(&act1.sa_mask,SIGUSR3);
	
	act2.sa_flags = 0;
	act2.sa_handler = sig_handler2;	
	sigaddset(&act2.sa_mask,SIGUSR1);
	sigaddset(&act2.sa_mask,SIGUSR3);
	
	act3.sa_flags = 0;
	act3.sa_handler = sig_handler3;	
	sigaddset(&act3.sa_mask,SIGUSR1);
	sigaddset(&act3.sa_mask,SIGUSR2);
	
	sigaction(SIGUSR1,&act1,NULL);
	sigaction(SIGUSR2,&act2,NULL);
	sigaction(SIGUSR3,&act3,NULL);

	//processing signal
	sigemptyset(&pending);
	sigemptyset(&sig123);
	sigemptyset(&sig000);
	sigaddset(&sig123, SIGUSR1);
	sigaddset(&sig123, SIGUSR2);
	sigaddset(&sig123, SIGUSR3);
	
	sigprocmask(SIG_BLOCK, &sig123, NULL);
	//
	if(setjmp(SCHEDULER) == 0)
		funct_5(1);
	Scheduler();
	//printf("%s\n",arr);
}
