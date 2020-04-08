#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
char list[4096][40];
int count = 0;
struct id_score{
	int id;
	int score;
};
void dfs(int start, int now, int limit, int player_num, int tmp[]){
    tmp[now] = start;
    now++;
    if(now == limit){
        char add[40]="";
        for(int i = 0; i < 8; i++){
            if(tmp[i]<10){
                add[strlen(add)] = tmp[i]+'0';
            }
            else{
                add[strlen(add)] = '1';
                add[strlen(add)] = tmp[i]-10+'0';
            }
            if(i!=7)
               add[strlen(add)] = ' ';
        }
		strcat(add,"\n");
        strcpy(list[count],add);
        count++;
        return;
    }
    for(int i = start+1; i <= player_num; i++){
        if(limit-now <= player_num - i+1 )
            dfs(i, now, limit, player_num, tmp);
    }
}
void generate_compete_list(int player_num){
	int tmp[8]={0};
	for(int i = 1; i <= player_num-8+1; i++)
		dfs(i,0,8,player_num,tmp);
	return;
}
int cmp(const void *a, const void *b){
	struct id_score *aa = (struct id_score*)a;
	struct id_score *bb = (struct id_score*)b;
	return (-aa -> score + bb -> score);
}
//###############################################################################################
int main(int argc, char **argv){
	
    if(argc != 3){
        fprintf(stderr,"input format error.\n");
        exit(0);
    }    
	int host_num = atoi(argv[1]);
    int player_num = atoi(argv[2]);
	
	//dfs list
	generate_compete_list(player_num);
	//
	srand(time(NULL));
	int random[11];
	for(int i = 1; i <= host_num; i++){
		random[i] = (rand()%65536);
		if(fork()==0){
			char tmp_id[16] = "";
			char tmp_key[16] = "";
			sprintf(tmp_id,"%d",i);
			sprintf(tmp_key,"%d",random[i]);		
			execlp("./host","./host",tmp_id,tmp_key,"0",NULL);
		}
	}
	//make fifo and open
	int fd[11];
	char fifo_list[11][16];
	for(int i = 1; i <= host_num; i++){
		sprintf(fifo_list[i],"Host%d.FIFO",i);
		unlink(fifo_list[i]);
		mkfifo(fifo_list[i],0777);
	}
	sprintf(fifo_list[0],"Host.FIFO");
    unlink(fifo_list[0]);
    mkfifo(fifo_list[0],0777);
    fd[0] = open(fifo_list[0],O_RDWR,777);
    dup2(fd[0],0);
    close(fd[0]);
	for(int i = 1; i <= host_num; i++){
		fd[i] = open(fifo_list[i],O_RDWR,777);
	}
	//assign
	int write_counter = 0;
	int read_counter = 0;
	struct id_score player[15];
	for(int i = 1; i <= player_num; i++){
		player[i].id = i;
		player[i].score = 0;
	}
	player[0].id = -1;
	player[0].score = -1;
	//init
	int init;
	if(host_num > count)
		init = count;
	else
		init = host_num;

	for(int i = 1; i <= init; i++){
		write(fd[i],list[write_counter],strlen(list[write_counter]));
		fsync(fd[i]);
		write_counter++;
	}
	
	while(read_counter < count){
		int tmp_key;
		scanf("%d",&tmp_key);
		for(int i = 0; i < 8; i++){
			int hid;
			int rk;
			scanf(" %d%d",&hid,&rk);
			player[hid].score += (8-rk);
		}
		read_counter++;
		int id;//check host_id
		for(int i = 1; i <= player_num; i++)
			if(random[i] == tmp_key){id = i;break;}
		if(write_counter < count){
			write(fd[id],list[write_counter],strlen(list[write_counter]));
			fsync(fd[id]);
			write_counter++;
		}
	}
	//printf("r:%d w:%d all:%d\n",read_counter,write_counter,count);
	//end message
	for(int i = 1; i <= host_num; i++){
		char end[30] = "-1 -1 -1 -1 -1 -1 -1 -1\n";
		write(fd[i], end, sizeof(end));
		fsync(fd[i]);
		close(fd[i]);
	}
	int status = 0;
	for(int i = 1; i <= host_num; i++){
		wait(&status);
	}
	unlink("Host.FIFO");
	for(int i = 1; i <= host_num; i++){
		char tmp[30];
		sprintf(tmp,"Host%d.FIFO",i);
		unlink(tmp);
	}

	//ranking
	qsort(player,player_num+1,sizeof(struct id_score),cmp);
	int rank[15];
	int rank_counter = 1;
	for(int i = 0; i < player_num; i++){
		rank[player[i].id] = rank_counter;
		if(i != player_num-1 && player[i].score > player[i+1].score)
			rank_counter++;
	}
	for(int i = 1; i <= player_num; i++){
		printf("%d %d\n",i,rank[i]);
	}
	return 0;
}
