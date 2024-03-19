#include <iostream>
#include "matmul_it.h"
#include <cstdio>

int softwaretest(uint32_t[MAX_SIZE][MAX_SIZE],uint32_t[MAX_SIZE][MAX_SIZE],uint32_t[MAX_SIZE][MAX_SIZE]);
int main(){

	 /*uint32_t a[MAX_SIZE][MAX_SIZE]={{1,2,3},
				          	    {4,5,6}};
		 uint32_t b[MAX_SIZE][MAX_SIZE]={{1,4},
						  	    {7,5},
								{3,6}};*/
		uint32_t out[MAX_SIZE][MAX_SIZE];
	uint32_t a[MAX_SIZE][MAX_SIZE] ;
	uint32_t b[MAX_SIZE][MAX_SIZE] ;
	for(int i = 0; i<MAX_SIZE;i++){
		for(int j = 0;j<MAX_SIZE;j++){
			a[i][j]=1;
			b[i][j]=1;
		}
	}
    const int iter = 4;
	FILE *fp;
matmul_it(*a,*b,*out,MAX_SIZE,MAX_SIZE,MAX_SIZE,iter);
softwaretest(a,b,out);
fp=fopen("mulmat.dat","w");

for(int i=0;i<ROWA;i++){
	for(int j=0;j<COLB;j++){
		fprintf(fp,"%d ",out[i][j]);
	}
	fprintf(fp,"\n");
	printf("\n");
}

}
//////// Software matrix multiplication /////////////
int softwaretest(uint32_t a[MAX_SIZE][MAX_SIZE],uint32_t b[MAX_SIZE][MAX_SIZE],uint32_t hw_out[MAX_SIZE][MAX_SIZE]){
	int sw_out[ROWA][COLB]={0};
	for(int i=0;i<ROWA;i++){
		for(int j=0;j<COLB;j++){
			for(int z=0;z<COLA;z++){
				sw_out[i][j] += a[i][z] * b[z][j];


}
			printf("%d ",hw_out[i][j]);
			if(sw_out[i][j]!=hw_out[i][j]){
				printf("\nMismatch\n");
				return 1;
			}
}
			printf("\n");
}
	printf("Test passed, grats");
	return 0;
}

