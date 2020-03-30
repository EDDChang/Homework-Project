#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
struct id_score{
	int id;
	int score;
};
int cmp(const void *a,const void *b){
	return ((*(struct id_score*)b).score - (*(struct id_score*)a).score);
}
int main(int argc, char **argv){
	if(argc != 4){
		fprintf(stderr,"input format error.\n");
		exit(0);
	}
	int host_id = atoi(argv[1]);
	int random_key = atoi(argv[2]);
	int depth = atoi(argv[3]);
//############### construct IPC : write to Host.FIFO, scanf form Hosti.FIFO  ####################
	char fifo[20];
	sprintf(fifo,"Host%d.FIFO",host_id);
	int read_fd = open(fifo,O_RDONLY);
	//close(read_fd);
	//##############################################################################################
	FILE *r_write[2];
	FILE *r_read[2];
	FILE *c_write[2];
	FILE *c_read[2];
	FILE *l_write[2];
	FILE *l_read[2];
	int pipe_rtoc[2][2],pipe_ctor[2][2];
	int pipe_ctol[2][2],pipe_ltoc[2][2];
	int pipe_ltop[2][2],pipe_ptol[2][2];

	for(int ii = 0; ii < 2; ii++){
		int rtoc_fd,ctor_fd;
		rtoc_fd = pipe(pipe_rtoc[ii]);
		ctor_fd = pipe(pipe_ctor[ii]);
		if(rtoc_fd == -1 || ctor_fd == -1){
			perror("pipe error:");
			exit(EXIT_FAILURE);
		}
		pid_t pid = fork();	
		if(pid == -1){
			perror("fork error:");
			exit(EXIT_FAILURE);
		}
		else if(pid == 0){//child
			
			close(pipe_rtoc[ii][1]);
			dup2(pipe_rtoc[ii][0],0);
			close(pipe_rtoc[ii][0]);
			                                                                                                                                                                            
			close(pipe_ctor[ii][0]);
			dup2(pipe_ctor[ii][1],1);
			close(pipe_ctor[ii][1]);

			for(int j = 0; j < 2; j++){
				int ctol_fd, ltoc_fd;
				ctol_fd = pipe(pipe_ctol[j]);
				ltoc_fd = pipe(pipe_ltoc[j]);
				//fprintf(stderr,"%d %d %d %d\n",pipe_ctol[j][0],pipe_ctol[j][1],pipe_ltoc[j][0],pipe_ltoc[j][1]);
				//fprintf(stderr,"%d\n",pipe_ctol[j][0]);
				if(ltoc_fd == -1 || ctol_fd == -1){
					perror("pipe error:");
					exit(EXIT_FAILURE);
				}
				pid_t ppid = fork();
				if(ppid == -1){
					perror("fork error:");
					exit(EXIT_FAILURE);
				}
				else if(ppid == 0){///leaf
					
					close(pipe_ctol[j][1]);
					dup2(pipe_ctol[j][0],0);
					close(pipe_ctol[j][0]);

					close(pipe_ltoc[j][0]);
					dup2(pipe_ltoc[j][1],1);
					close(pipe_ltoc[j][1]);
					
					while(1){
						
						int playerid[2];
						scanf("%d%d",&playerid[0],&playerid[1]);
						//fprintf(stderr,"leaf:%d %d\n",playerid[0],playerid[1]);
						if(playerid[0] == -1){
							break;
						}	
						
						for(int k = 0; k < 2; k++){
							//fprintf(stderr,"ii:%d j:%d k:%d\n",ii,j,k);
							int ltop_fd, ptol_fd;
							ltop_fd = pipe(pipe_ltop[k]);
							ptol_fd = pipe(pipe_ptol[k]);
							if(ltop_fd == -1 || ptol_fd == -1){
								perror("pipe error:");
								exit(EXIT_FAILURE);
							}
							pid_t pppid = fork();
							if(pppid == -1){
								perror("fork error:");
								exit(EXIT_FAILURE);
							}
							else if(pppid == 0){//player
								
								close(pipe_ltop[k][1]);
			                    dup2(pipe_ltop[k][0],0);
						        close(pipe_ltop[k][0]);

								close(pipe_ptol[k][0]);
								dup2(pipe_ptol[k][1],1);
			                    close(pipe_ptol[k][1]);
								
								char player_id[8];
								sprintf(player_id,"%d\n",playerid[k]);
								execlp("./player","./player",player_id,NULL);
							}
							close(pipe_ltop[k][0]);
							l_write[k] = fdopen(pipe_ltop[k][1],"w");
							close(pipe_ptol[k][1]);
							l_read[k] = fdopen(pipe_ptol[k][0],"r");
						}
						int l_count = 0;
						while(l_count < 10){
							int tmp_id[2];
							int tmp_money[2];
							for(int i = 0; i < 2; i++){
								fscanf(l_read[i],"%d%d",&tmp_id[i],&tmp_money[i]);
							}
							//fprintf(stderr,"leaf:%d %d %d %d\n",tmp_id[0],tmp_money[0],tmp_id[1],tmp_money[1]);
							int win_id,win_money;
							if(tmp_money[0] > tmp_money[1]){
								win_id = tmp_id[0];
								win_money = tmp_money[0];
							}
							else if(tmp_money[0] < tmp_money[1]){
								win_id = tmp_id[1];
                                win_money = tmp_money[1];   
							}
							printf("%d %d\n",win_id,win_money);
							fflush(stdout);
							fsync(fileno(stdout));
							if(l_count < 9){
								
								scanf("%d",&win_id);
								for(int i = 0; i < 2; i++){
									fprintf(l_write[i],"%d\n",win_id);
									fflush(l_write[i]);
									fsync(fileno(l_write[i]));
								}
							}
							l_count++;
						}
						for(int i = 0; i < 2; i++)
							wait(NULL);
					}
					for(int i = 0; i < 2; i++){
						fclose(l_write[i]);
						fclose(l_read[i]);
					}  
					exit(0);
				}
				close(pipe_ctol[j][0]);
				c_write[j] = fdopen(pipe_ctol[j][1],"w");
				close(pipe_ltoc[j][1]);
				c_read[j] = fdopen(pipe_ltoc[j][0],"r");
			}
			while(1){
	            int send_leaf[4];
				scanf("%d%d%d%d",&send_leaf[0],&send_leaf[1],&send_leaf[2],&send_leaf[3]);
				
				for(int i = 0; i < 2; i++){
					fprintf(c_write[i],"%d %d\n",send_leaf[0+2*i],send_leaf[1+2*i]);
					fflush(c_write[i]);
					fsync(fileno(c_write[i]));
				}
				if(send_leaf[0] == -1)	break;
				
				int c_round = 0;
				while(c_round < 10){
					int tmp_id[2];
					int tmp_money[2];
					for(int i = 0; i < 2; i++){						
						fscanf(c_read[i],"%d%d",&tmp_id[i],&tmp_money[i]);
					}
					//fprintf(stderr,"child:%d %d %d %d\n",tmp_id[0],tmp_money[0],tmp_id[1],tmp_money[1]);
					int win_id,win_money;
					if(tmp_money[0]>tmp_money[1]){
						win_id = tmp_id[0];
						win_money = tmp_money[0];
					}
					else if(tmp_money[0]<tmp_money[1]){
						win_id = tmp_id[1];
						win_money = tmp_money[1];
					}
					printf("%d %d\n",win_id,win_money);
					fflush(stdout);
					fsync(fileno(stdout));
					if(c_round != 9){

						scanf("%d",&win_id);
						for(int i = 0; i < 2; i++){
							fprintf(c_write[i],"%d\n",win_id);
							fflush(c_write[i]);
							fsync(fileno(c_write[i]));
						}
					}
					c_round++;
				}
			}
			for(int i = 0; i < 2; i++){
				fclose(c_write[i]);
				fclose(c_read[i]);
			}  
			for(int i = 0; i < 2; i++)
				wait(NULL);
			exit(0);
		}
		close(pipe_rtoc[ii][0]);
		r_write[ii] = fdopen(pipe_rtoc[ii][1],"w");
		close(pipe_ctor[ii][1]);
		r_read[ii] = fdopen(pipe_ctor[ii][0],"r");		
	}
	/*####### root's task 
	for(int i = 0 ;i < 2; i++)
		for(int j = 0; j < 2; j++)
			fprintf(stderr,"pipe_rtoc[%d][%d]=%d\n",i,j,pipe_rtoc[i][j]);
	for(int i = 0 ;i < 2; i++)
        for(int j = 0; j < 2; j++)
            fprintf(stderr,"pipe_ctor[%d][%d]=%d\n",i,j,pipe_ctor[i][j]); 
	for(int i = 0 ;i < 2; i++)
        for(int j = 0; j < 2; j++)
            fprintf(stderr,"pipe_ctol[%d][%d]=%d\n",i,j,pipe_ctol[i][j]);
	for(int i = 0 ;i < 2; i++)
        for(int j = 0; j < 2; j++)
            fprintf(stderr,"pipe_ltoc[%d][%d]=%d\n",i,j,pipe_ltoc[i][j]);*/
	while(1){
		struct id_score play_list[8];
		dup2(read_fd,0);
	    scanf("%d%d%d%d%d%d%d%d",&play_list[0].id,&play_list[1].id,&play_list[2].id,&play_list[3].id,&play_list[4].id,&play_list[5].id,&play_list[6].id,&play_list[7].id);                                  
		int map[15];
	    for(int i = 0; i < 8; i++){
	        map[play_list[i].id] = i;
			play_list[i].score = 0;
	    }   	
		for(int i = 0; i < 2; i++){
			fprintf(r_write[i],"%d %d %d %d\n",play_list[0+4*i].id,play_list[1+4*i].id,play_list[2+4*i].id,play_list[3+4*i].id);
			fflush(r_write[i]);
			fsync(fileno(r_write[i]));
		}
		int r_round = 0;
		while(r_round < 10){
			//printf("r_round:%d\n",r_round);
			int tmp_id[2];
			int tmp_money[2];
			for(int i = 0; i < 2; i++){
				fscanf(r_read[i],"%d%d",&tmp_id[i],&tmp_money[i]);
			}
			//fprintf(stderr,"root:%d %d %d %d\n",tmp_id[0],tmp_money[0],tmp_id[1],tmp_money[1]);
			int win_id, win_money;
			if(tmp_money[0] > tmp_money[1]){
				win_id = tmp_id[0];
				win_money = tmp_money[0];
			}
			else if(tmp_money[0] < tmp_money[1]){
				win_id = tmp_id[1];
				win_money = tmp_money[1];
			}
			//fprintf(stderr,"%d\n",map[win_id]);
			//fprintf(stderr,"root_winner:%d\n",winner);
			play_list[map[win_id]].score++;	
			char mes[10];
			//fprintf(stderr,"%s",mes);
			if(r_round < 9){
				//fprintf(stderr,"error\n");
				for(int i = 0; i < 2; i++){
					fprintf(r_write[i],"%d\n",win_id);
					fflush(r_write[i]);
					fsync(fileno(r_write[i]));
				}
			}
			r_round++;
		}

		int rank[8] = {0};
		for(int i = 0; i < 8; i++){
			rank[i] = 1;
			for(int j = 0; j < 8; j++)
				if(play_list[j].score > play_list[i].score)
					rank[i]++;
		}
		int write_fifo = open("Host.FIFO",O_WRONLY);
		dup2(write_fifo,1);
		printf("%d\n",random_key);
		for(int i = 0; i < 8; i++){
			printf("%d %d\n",play_list[i].id,rank[i]);
			//fprintf(stderr,"%d %d\n",play_list[i].id,play_list[i].score);
		}
		fflush(stdout);
		fsync(fileno(stdout));
	}
	for(int i = 0; i < 2; i++){
		fclose(r_write[i]);
		fclose(r_read[i]);
	}
	for(int i = 0; i < 2; i++)
		wait(NULL);
	exit(0);
}		
