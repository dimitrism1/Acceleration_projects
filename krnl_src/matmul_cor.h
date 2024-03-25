#include <iostream>
#define ROWA 3		//used only for the testbench
#define COLA 4
#define COLB 3

#define MAX_SIZE 256
extern "C"{

/*void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]);//,uint32_t rowa,uint32_t cola,uint32_t colb);
}*/
void matmul_cor(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col,uint32_t iter);
}
