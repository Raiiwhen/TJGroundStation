#include "mat.h"
#include <string.h>
#include <stdio.h>

float ram_mat[512];//2KB ram
float ram_tmp[32]; //temporary data
long ram_mat_ptr;
long ram_tmp_ptr;

/*example*/
matrix mat_ERR={
	.row = 1,
	.col = 1,
	.data = NULL,
	.sta = MAT_ERR
};
/*end example*/

int mat_init(matrix* mat, int row, int col){
	ram_mat_ptr += (row*col);
	if(ram_mat_ptr>512)return 1;
	
	mat->col = col;
	mat->row = row;
	mat->data = ram_mat + ram_mat_ptr;
	memset(mat->data,0,col*row);
	
	return 0;
}

int mat_reinit(matrix* mat, int row, int col){
	mat->col = col;
	mat->row = row;
	mat->data = ram_tmp + ram_tmp_ptr;
	memset(mat->data,0,col*row);
	
	ram_tmp_ptr += (row*col);
	if(ram_mat_ptr>512)ram_tmp_ptr=0;
	
	return 0;
}

int mat_fill(matrix* mat, float* data){
	memcpy(mat->data, data, mat->row * mat->col * 4);
	
	return 0;
}

matrix mat_unit(int m){
	int i;
	matrix mat = {0};
	mat_reinit(&mat,m,m);
	for(i=0; i<m; i++)
		*(mat.data + i * mat.col + i) = 1;
	return mat;
}

matrix mat_add(matrix A, matrix B){
	int i,j;
	matrix mat = {0};
	if(A.col != B.col || A.row != B.row)return mat_ERR;
	
	mat_reinit(&mat, A.row, A.col);
	for(i=0; i < mat.row; i++){
		for(j=0; j < mat.col; j++)
			*(mat.data + i * mat.col + j) = 
												*(A.data + i * A.col + j)
											+ *(B.data + i * B.col + j);
	}
	return mat;
}

matrix mat_sub(matrix A, matrix B){
	int i,j;
	matrix mat = {0};
	if(A.col != B.col || A.row != B.row)return mat_ERR;
	
	mat_reinit(&mat, A.row, A.col);
	for(i=0; i < mat.col; i++){
		for(j=0; j < mat.row; j++)
			*(mat.data + i * mat.col + j) = 
												*(A.data + i * A.col + j)
											- *(B.data + i * B.col + j);
	}
	return mat;
}

matrix mat_mult(matrix A, matrix B){
	int i,j,k;
	float tmp;
	matrix mat = {0};
	if(A.row != B.col)return mat_ERR;
	
	mat_reinit(&mat, A.row, B.col);
	for(i=0; i < mat.row; i++){
		for(j=0; j < mat.col; j++){
			for(k=0;k < A.row;k++){
				tmp = 
						*(A.data + i * A.col + j)
					* *(B.data + i * B.col + j);
			}
			*(mat.data + i * mat.col + j) = tmp;
		}
	}
	return mat;
}

matrix mat_tran(matrix A){
	int i, j;
	matrix mat = {0};

	mat_reinit(&mat,A.col,A.row);
	for(i=0; i<mat.row; i++){
		for(j=0; j<mat.col; j++)
				*(mat.data + i * mat.col + j) = 
									*(A.data + j * A.col + i);
	}
	return mat;
}

matrix mat_invr(matrix A){
	int i,j;
	float tmp;
	matrix mat;
	for(i=0;i<mat.row;i++){
//		for(j=0;j<mat.col;j++)
//			tmp += (mat.data[i][j]) * (mat.data[i][j]);
	}
	if(tmp==0)return mat_ERR;
	mat_reinit(&mat,A.row,A.col);
	
	return mat;
}

char* mat_put(matrix mat, char* Dst){
	int i,j;
	char buff[7] = {0};
	sprintf(Dst,"MAT %d x %d",mat.row, mat.col);
	sprintf(buff,"[0x%04X]\r\n", mat.sta);
	strcat(Dst,buff);
	for(i=0; i < mat.row; i++){
		for(j=0; j < mat.col; j++){
			sprintf(buff, "%.3f\t", *(mat.data + i * mat.col + j));
			strcat(Dst,buff);
		}
		strcat(Dst,"\r\n");
	}
	return Dst;
}
