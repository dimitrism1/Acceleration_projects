#include <iostream>
#define ROWA 64			//used only in the testbench
#define COLA 64
#define COLB 64

#define MAX_SIZE 256
extern "C"{

/*void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]);//,uint32_t rowa,uint32_t cola,uint32_t colb);
}*/
void matmul(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col);
}
