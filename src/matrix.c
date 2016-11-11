#include "matrix.h"


matrix_t* init_matrix(unsigned int rows, unsigned int columns) {
	matrix_t* m = malloc(sizeof(matrix_t));
	
	m->rows = rows;
	m->columns = columns;

	m->matrix = malloc(sizeof(double*) * rows);

	for (int i = 0; i < rows; i++) 
		m->matrix[i] = malloc(sizeof(double) * columns);

	return m;
}

matrix_t* matrix_vector_product(matrix_t* m, matrix_t* vec) {
	
	matrix_t* result = init_matrix(vec->rows, 1);
	int i, j;
		
	for (i = 0; i < m->rows; i++) {
		result->matrix[i][0] = 0;

		for (j = 0; j < m->columns; j++) {
			result->matrix[i][0] += m->matrix[i][j] * vec->matrix[j][0];
		}
	}
	return result;
}

int vector_scalar_addition (matrix_t* m, double scalar) {	
	for(int i = 0; i < m->rows; i++) 
		m->matrix[i][0] += scalar;	
	return 0;
}

int function_on_vector (matrix_t* vec, double (*f)(double)) {
	for (int i = 0; i < vec->rows; i++) 
		vec->matrix[i][0] = (*f)(vec->matrix[i][0]);
	return 0;
}


int transpose (matrix_t** m) {
	matrix_t* old_matrix = *m;
	matrix_t* new_matrix = init_matrix((*m)->columns, (*m)->rows);
	
	for (int i = 0; i < new_matrix->columns; i++) {
		for (int j = 0; j < new_matrix->rows; j++) {
			new_matrix->matrix[j][i] = old_matrix->matrix[i][j]; 
		}
	}

	/* Free memory of old matrix and point m to new one */
	free_matrix(old_matrix);
	*m = new_matrix;
	return 0;
}

matrix_t* random_matrix (unsigned int rows, unsigned int columns, double interval) {
	srand(time(NULL));
	matrix_t* random_matrix = init_matrix(rows, columns);
	double div = RAND_MAX / (interval * 2);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			random_matrix->matrix[i][j] = -interval + (rand() / div);
		}
	}	
	return random_matrix;
}


int free_matrix (matrix_t* m) {
	
	for (int i = 0; i < m->rows; i++)
		free(m->matrix[i]);
	free(m->matrix);
	free(m);
}
