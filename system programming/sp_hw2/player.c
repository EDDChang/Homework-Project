#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr,"Input format error.\n");
		exit(0);
	}
	int player_id = atoi(argv[1]);
	int winner_id;
	int round = 0;
	while(round < 10){	
		if(round !=0){
			scanf("%d",&winner_id);
		}
		printf("%d %d\n",player_id,player_id*100);
		fflush(stdout);
		round++;
	}
	exit(0);
}
