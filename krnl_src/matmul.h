#include <iostream>
#define ROWA 256	//used only in the testbench
#define COLA 256
#define COLB 256

#define MAX_SIZE 256
extern "C"{

/*void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]);//,uint32_t rowa,uint32_t cola,uint32_t colb);
}*/
void matmul(uint32_t* a,uint32_t* b,uint32_t* out,int a_row,int a_col,int b_col);
}
