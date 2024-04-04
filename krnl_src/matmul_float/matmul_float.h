#include <iostream>
#define ROWA 256	//used only in the testbench
#define COLA 256
#define COLB 256

#define MAX_SIZE 256
extern "C"{

/*void matmul(float a[][COLA],float b[][COLB],float out[][COLB]);//,float rowa,float cola,float colb);
}*/
void matmul_float(float* a,float* b,float* out,int a_row,int a_col,int b_col);
}
