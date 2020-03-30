#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
int X[60000][784];
int Xt[784][60000];
int Y[60000][10];
double W[784][10];
double W_grad[784][10];
double y_hat[60000][10];
int X_test[10000][784];
int y_test[10000];
double predict[10000][10];
int num_thread;
double lr = 0.02;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* update(void *count){
	int part = 60000/num_thread;
	int begin = (int)count * part;
	int end = begin + part;
	for(int i = begin; i < end; i++){
		for(int k = 0; k < 784; k++){
			for(int j = 0; j < 10; j++){
				y_hat[i][j] += (double)X[i][k]*W[k][j];
			}
		}
	}
	pthread_exit(NULL);
}
void softmax(){
	double exp_sum = 0;
	for(int i = 0; i < 60000; i++){
		exp_sum = 0;
		double big = -1;
		for(int j = 0; j < 10; j++){
			if(y_hat[i][j]>big){
				big = y_hat[i][j];
			}
		}
		for(int j = 0; j < 10; j++){
			exp_sum += exp(y_hat[i][j]-big);
		}
		for(int j = 0; j < 10; j++){
			y_hat[i][j] = exp(y_hat[i][j]-big)/exp_sum;
			y_hat[i][j] -= Y[i][j];
		}
	}
	return;
}
void* w_grad(void* count){
	for(int i = (int)count*28; i < ((int)count + 1)*28; i++)
		for(int k = 0; k < 60000; k++)
			for(int j = 0; j < 10; j++){
				W_grad[i][j] += Xt[i][k] * y_hat[k][j];
			}
	pthread_exit(NULL);
}
void new_w(){
	for(int i = 0; i < 784; i++){
		for(int j = 0; j < 10; j++){
			W[i][j] -= (lr*W_grad[i][j]);
		}
	}
}
int main(int argc, char **argv){
	num_thread = atoi(argv[4]);
	FILE *fp_x, *fp_y, *fp_t, *fp_yt;
	fp_x = fopen(argv[1], "rb");
	fp_y = fopen(argv[2], "rb");			
	fp_t = fopen(argv[3], "rb");
	//fp_yt = fopen(argv[4],"rb");
	/*for(int i = 0; i < 10000; i++){
		fread(&y_test[i],1,1,fp_yt);
 	}*/
	for(int i = 0 ; i < 60000 ; i++){
		for(int j = 0 ; j < 784 ; j++){
			fread(&X[i][j],1,1,fp_x);
			Xt[j][i] = X[i][j];
		}
	}
	for(int i = 0; i < 10000; i++){
		for(int j = 0; j < 784; j++)
			fread(&X_test[i][j],1,1,fp_t);
	}
	for(int i = 0; i < 784; i++)
		for(int j = 0; j < 10 ;j++)
			W[i][j] = 0.0;
	for(int i = 0; i < 60000; i++){
		int label;
		fread(&label,1,1,fp_y);
		Y[i][label] = 1;
	}
	pthread_t p[4096];
	int iter;
	if(num_thread == 1)
		iter = 75;
	else
		iter = 180;
	for(int i = 0; i < iter; i++){
		/////////////////mat mul/////////
		for(int ii = 0; ii < 60000; ii++)
			for(int j = 0; j < 10; j++)
				y_hat[ii][j] = 0;
		for(int j = 0; j < num_thread; j++){ 
			pthread_create(&p[j], NULL, update,j);			
		}
		for(int j = 0; j < num_thread; j++)
			pthread_join(p[j],NULL);
		//////////
		softmax();
		///////////////////grad/////////////
		for(int j = 0; j < 28; j++){
			pthread_create(&p[j],NULL,w_grad,j);
		}
		for(int j = 0; j < 28; j++)
			pthread_join(p[j],NULL);
		///////
		new_w();
		//printf("%d\n",i);
		/*for(int ii = 0; ii < 10; ii++)
		for(int j = 0; j < 10000; j++)
			predict[ii][j] = 0;
		for(int ii = 0; ii < 10000; ii++){
			for(int k = 0; k < 784; k++){
				for(int j = 0; j < 10; j++){
					predict[ii][j] += (double)X_test[ii][k]*W[k][j];
				//printf("%.1lf ",y_hat[i][j]);
				}
			}
		}
		int outcome[10000];
		for(int ii = 0; ii < 10000; ii++){
			double big = -1;
			int label = -1;
			for(int j = 0; j < 10; j++){
				if(predict[ii][j] > big){
					big = predict[ii][j];
					label = j;
				}
			}
			outcome[ii] = label;
		}
		int acc = 0;
		for(int ii = 0; ii < 10000; ii++){
			if(outcome[ii] == y_test[ii])
				acc++;
		}
		double a = (double)acc/10000;
		printf("%d : %lf\n",i,a);*/
	}
/////////////////////test////////////////
	for(int ii = 0; ii < 10; ii++)
		for(int j = 0; j < 10000; j++)
			predict[ii][j] = 0;
	for(int ii = 0; ii < 10000; ii++){
		for(int k = 0; k < 784; k++){
			for(int j = 0; j < 10; j++){
				predict[ii][j] += (double)X_test[ii][k]*W[k][j];
				//printf("%.1lf ",y_hat[i][j]);
			}
		}
	}
	int outcome[10000];
	for(int ii = 0; ii < 10000; ii++){
		double big = -1;
		int label = -1;
		for(int j = 0; j < 10; j++){
			if(predict[ii][j] > big){
				big = predict[ii][j];
				label = j;
			}
		}
		outcome[ii] = label;
	}
	FILE *fp = fopen("result.csv","w");
	fprintf(fp,"id,label\n");
	for(int i = 0; i < 10000; i++)
		fprintf(fp,"%d,%d\n",i,outcome[i]);
	/*int acc = 0;
	for(int ii = 0; ii < 10000; ii++){
		if(outcome[ii] == y_test[ii])
			acc++;
	}*/
	//double a = (double)acc/10000;
	//printf("%f\n",a);
	fclose(fp_x);
	fclose(fp_y);
	fclose(fp_t);
	//fclose(fp_yt);
	fclose(fp);
	return 0;
}
