#include <iostream>
#define ROWA 6			//used only in the testbench
#define COLA 4
#define COLB 8

#define MAX_SIZE 16
extern "C"{

/*void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]);//,uint32_t rowa,uint32_t cola,uint32_t colb);
}*/
void matmul(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col);
}
