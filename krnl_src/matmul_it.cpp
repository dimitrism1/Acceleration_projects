#include <iostream>
#include "matmul_it.h"
#include <cstring>




//void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]){//,uint32_t rowa,uint32_t cola,uint32_t colb ){
void matmul_it(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col,uint32_t iter){
int b_row=a_col;
int c_row=a_row;
int c_col=b_col;
int l=0;

const unsigned int c_size=MAX_SIZE;
/*int a_row=ROWA;
int a_col=COLA;
int b_row=ROWB;
int b_col=COLB;*/
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=a depth=6	///doesnt work without setting depth
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=b depth=6
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=out depth=6

int atemp[MAX_SIZE][MAX_SIZE];
int btemp[MAX_SIZE][MAX_SIZE];
int outtemp[MAX_SIZE][MAX_SIZE]={0};//={{0,0},
						  // {0,0}};
int zero[MAX_SIZE][MAX_SIZE]={0};
#pragma HLS ARRAY_PARTITION variable=atemp dim=1 type=complete
#pragma HLS ARRAY_PARTITION variable=btemp dim=2 type=complete
#pragma HLS ARRAY_PARTITION variable=outtemp dim=0 type=complete


/*#pragma HLS bind_storage type=RAM_S2P variable=atemp impl=LUTRAM
#pragma HLS bind_storage type=RAM_S2P variable=btemp impl=LUTRAM
#pragma HLS bind_storage type=RAM_S2P variable=outtemp impl=LUTRAM*/




/*loadarow:for(int i = 0;i<a_row;i++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
loadacol:	for(int j = 0;j<a_col;j++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
		atemp[i][j]=a[z];
		//printf("%d\n",a[z]);
		z++;

	}
}*/
/////////// Load a to local memory //////////////
loada:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
				if(j==a_col){
					i++;
					j=0;
				}
				//atemp[i][j]=(z<a_row*a_col) ? a[z] : 0;
				if(z<a_row*a_col){
				atemp[i][j]=a[z];
				}


}
//int k=0;
/*loadbrow:for(int i = 0;i<b_row;i++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
loadbcol:	for(int j=0;j<b_col;j++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
		btemp[i][j]=b[k];
		//printf("%d\n",b[k]);
		k++;
		//printf("%d\n",btemp[i][j]);

	}
}*/

/////////// Load b to local memory //////////////

loadb:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
				if(j==b_col){
					i++;
					j=0;
				}
				if(z<b_row*b_col){
				btemp[i][j]=b[z];
				}
				//btemp[i][j]=(z<b_row*b_col) ? b[z] : 0;
}
/*for(int i=0;i<b_row;i++){
	for(int j=0;j<b_col;j++){
		printf("%d\n",btemp[i][j]);
	}
}*/


/////////// Matrix multiplication //////////////
iter: for(uint32_t rep_c=0; rep_c < iter; rep_c++){
#pragma HLS LOOP_TRIPCOUNT min=10000000 max=10000000
	initzero:for(int a = 0; a < MAX_SIZE; a++){

			{
			for(int b = 0; b < MAX_SIZE; b++)
			outtemp[a][b]=0;
		}
		}
	rowa:	for (uint32_t i=0;i<a_row;i++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
			//if(i<a_row){
colb:		for(uint32_t j=0;j<b_col;j++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
	//if(j<b_col){


cola:			for(uint32_t z=0;z<MAX_SIZE;z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size

				outtemp[i][j]+=(z<a_col) ? (atemp[i][z]*btemp[z][j]) : 0;


		}
			}
			}
}




////////////Send results to global memory /////////////
sendout:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
//#pragma HLS UNROLL
	if(j==c_col){
		i++;
		j=0;
	}
	out[z]=(z<c_row*c_col) ? outtemp[i][j] : 0;
	//outtemp[i][j] = 0;
}
//std::memcpy(outtemp,zero,MAX_SIZE*MAX_SIZE);
}


