#include <iostream>
#include "matmul.h"
#include <cstdio>

int main(){

	 uint32_t a[MAX_SIZE][MAX_SIZE]={{1,2,3},
				          	    {4,5,6}};
		 uint32_t b[MAX_SIZE][MAX_SIZE]={{1,4},
						  	    {7,5},
								{3,6}};
		uint32_t out[MAX_SIZE][MAX_SIZE]={{0,0},
								 {0,0}};


	FILE *fp;
	uint32_t k[COLA]={3,2,1};

	//uint32_t *ptr=&a[0][0];*/

matmul(*a,*b,*out,MAX_SIZE,MAX_SIZE,MAX_SIZE);
//matmul(a,b,out);
fp=fopen("mulmat.dat","w");
//for(int i=0;i<COLA*ROWA;i++){
	//printf("%d\n",*ptr+i);
//}
for(int i=0;i<ROWA;i++){
	for(int j=0;j<COLB;j++){
		//printf("%d \n",sizeof(a)/sizeof(a[0]));
		printf("%d ",out[i][j]);
		//printf("%d ",b[i][j]);
		fprintf(fp,"%d ",out[i][j]);
	}
	fprintf(fp,"\n");
	printf("\n");
}
if(out[0][0]==24&out[0][1]==32&out[1][0]==57&out[1][1]==77){
	printf("Test passed,grats");
}
else{
	return 0;
}




}
