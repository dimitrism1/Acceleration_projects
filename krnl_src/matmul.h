#include <iostream>
#define ROWA 2
#define COLA 3
#define ROWB 3
#define COLB 2

#define MAX_SIZE 8
extern "C"{

/*void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]);//,uint32_t rowa,uint32_t cola,uint32_t colb);
}*/
void matmul(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col);
}
