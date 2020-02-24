#ifndef _MATH_H_
#define _MATH_H_


enum MAT_STA{
	MAT_OK=0,
	MAT_ERR,
	MAT_OVR,
	MAT_UNIT
};

typedef struct{
	int row;
	int col;
	float* data;
	enum MAT_STA sta;
}matrix;

int mat_init(matrix* mat, int row, int col);
int mat_fill(matrix* mat, int row, int col, float* data);
char* mat_put(matrix mat, char* Dst);

matrix mat_unit(int m);
matrix mat_add(matrix A, matrix B);
matrix mat_sub(matrix A, matrix B);
matrix mat_mult(matrix A, matrix B);
matrix mat_tran(matrix A);
matrix mat_invr(matrix A);

#endif
