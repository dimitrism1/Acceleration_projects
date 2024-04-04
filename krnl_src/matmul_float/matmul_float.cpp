#include <iostream>
#include "matmul_float.h"




//void matmul(float a[][COLA],float b[][COLB],float out[][COLB]){//,float rowa,float cola,float colb ){
void matmul_float(float* a,float* b,float* out,int a_row,int a_col,int b_col){
int b_row = a_col;
int c_row = a_row;
int c_col = b_col;

const unsigned int c_size = MAX_SIZE;

#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=a depth=c_size/2	///co-simulation doesnt work without setting depth
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=b depth=c_size/2
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=out depth=c_size/2

int atemp[MAX_SIZE][MAX_SIZE];
int btemp[MAX_SIZE][MAX_SIZE];
int outtemp[MAX_SIZE][MAX_SIZE];

#pragma HLS ARRAY_PARTITION variable=atemp dim=2 cyclic factor = c_size/2
#pragma HLS ARRAY_PARTITION variable=btemp dim=1 cyclic factor = c_size/2
//#pragma HLS ARRAY_PARTITION variable=outtemp dim=2 cyclic factor=c_size







/////////// Load a to local memory //////////////
loada:for(int i = 0,j = 0,z = 0;z < MAX_SIZE*MAX_SIZE; j++, z++){
				if(j == a_col){
					i++;
					j = 0;
				}
				atemp[i][j] = a[z];


}



/////////// Load b to local memory //////////////

loadb:for(int i = 0,j = 0,z = 0; z < MAX_SIZE*MAX_SIZE; j++ , z++){
				if(j == b_col){
					i++;
					j = 0;
				}
				btemp[i][j] = b[z];

}




/////////// Matrix multiplication //////////////

rowa:	for (int i = 0; i < MAX_SIZE ; i++){
colb:		for(int j = 0; j < MAX_SIZE; j++){
    //// The pipeline pragma is used by default for loop limits below 64. Needs to be specified for MAX_SIZE > 64 ////////
	#pragma HLS PIPELINE II=1
	int temp=0;
cola:			for(int z = 0; z < MAX_SIZE ; z++){

				temp += (z < a_col && j < b_col && i < a_row) ? (atemp[i][z] * btemp[z][j]) : 0;
				outtemp[i][j] = temp;



		}
			}
			}



////////////Send results to global memory /////////////
sendout:for(int i = 0, j = 0,z = 0; z < MAX_SIZE * MAX_SIZE ; j++, z++){
//#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
	if(j == c_col){
		i++;
		j = 0;
	}
	out[z] = outtemp[i][j];
}


}
