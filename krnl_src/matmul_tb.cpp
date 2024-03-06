#include <iostream>
#include "matmul.h"
#include <cstdio>

int softwaretest(uint32_t[MAX_SIZE][MAX_SIZE],uint32_t[MAX_SIZE][MAX_SIZE],uint32_t[MAX_SIZE][MAX_SIZE]);
int main(){

	 uint32_t a[MAX_SIZE][MAX_SIZE]={{1,2,3},
				          	    {4,5,6}};
		 uint32_t b[MAX_SIZE][MAX_SIZE]={{1,4},
						  	    {7,5},
								{3,6}};
		uint32_t out[MAX_SIZE][MAX_SIZE]={{0,0},
								 {0,0}};


	FILE *fp;
	//uint32_t k[COLA]={3,2,1};

	//uint32_t *ptr=&a[0][0];*/

matmul(*a,*b,*out,MAX_SIZE,MAX_SIZE,MAX_SIZE);
softwaretest(a,b,out);
//matmul(a,b,out);
fp=fopen("mulmat.dat","w");
//for(int i=0;i<COLA*ROWA;i++){
	//printf("%d\n",*ptr+i);
//}
for(int i=0;i<ROWA;i++){
	for(int j=0;j<COLB;j++){
		//printf("%d \n",sizeof(a)/sizeof(a[0]));
		//printf("%d ",out[i][j]);
		//printf("%d ",b[i][j]);
		fprintf(fp,"%d ",out[i][j]);
	}
	fprintf(fp,"\n");
	printf("\n");
}
/*if(out[0][0]==24&out[0][1]==32&out[1][0]==57&out[1][1]==77){
	printf("Test passed,grats");
}
else{
	return 33;
}*/




}

int softwaretest(uint32_t a[MAX_SIZE][MAX_SIZE],uint32_t b[MAX_SIZE][MAX_SIZE],uint32_t hw_out[MAX_SIZE][MAX_SIZE]){
	int sw_out[ROWA][COLB]={0};
	for(int i=0;i<ROWA;i++){
		for(int j=0;j<COLB;j++){
			for(int z=0;z<COLA;z++){
				sw_out[i][j] += a[i][z] * b[z][j];


				//sw_out[i*COLB + j]=[i*COLB + j]+= a[i*COLA + z]*b[z*COLB + j];
				/*if(sw_out[i][j] != hw_out[i][j]){
					printf("%",sw_out);
					return 1;
				}*/

}
			printf("%d ",sw_out[i][j]);
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
