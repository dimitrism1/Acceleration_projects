#include <iostream>
#include "matmul_float.h"
#include <cstdio>

int softwaretest(float[MAX_SIZE][MAX_SIZE],float[MAX_SIZE][MAX_SIZE],float[MAX_SIZE][MAX_SIZE]);
int main(){

	 float a[MAX_SIZE][MAX_SIZE];
	 //{{1,2,3},				          	    {4,5,6}};
		 float b[MAX_SIZE][MAX_SIZE];
		 //{{1,4},				  	    {7,5},			{3,6}};
		float out[MAX_SIZE][MAX_SIZE];
		//{{0,0},								 {0,0}};

		for(int i = 0; i<MAX_SIZE;i++){
				for(int j = 0;j<MAX_SIZE;j++){
					a[i][j]=1;
					b[i][j]=1;
				}
			}

	FILE *fp;
matmul_float(*a,*b,*out,ROWA,COLA,MAX_SIZE);
softwaretest(a,b,out);
fp=fopen("mulmat.dat","w");

for(int i=0;i<ROWA;i++){
	for(int j=0;j<COLB;j++){
		fprintf(fp,"%f ",out[i][j]);
	}
	fprintf(fp,"\n");
	printf("\n");
}

}
//////// Software matrix multiplication /////////////
int softwaretest(float a[MAX_SIZE][MAX_SIZE],float b[MAX_SIZE][MAX_SIZE],float hw_out[MAX_SIZE][MAX_SIZE]){
	int sw_out[ROWA][COLB]={0};
	for(int i = 0; i < ROWA; i++){
		for(int j = 0; j < COLB; j++){
			for(int z = 0; z < COLA; z++){
				sw_out[i][j] += a[i][z] * b[z][j];

}
			printf("%f ",hw_out[i][j]);
			if(sw_out[i][j] != hw_out[i][j]){
				printf("\nMismatch\n");
				return 1;
			}
}
			printf("\n");
}
	printf("Test passed, grats");
	return 0;
}
