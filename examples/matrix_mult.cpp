/**
 * matrix_mult.cpp
 * Example C++ program for matrix multiplication to be compiled by the PIM compiler
 */

#include <iostream>

/**
 * Matrix multiplication function
 * 
 * @param A First matrix (rows x common)
 * @param B Second matrix (common x cols)
 * @param C Result matrix (rows x cols)
 * @param rows Number of rows in matrix A and C
 * @param cols Number of columns in matrix B and C
 * @param common Common dimension (columns of A, rows of B)
 */
void matrixMultiply(int* A, int* B, int* C, int rows, int cols, int common) {
    // Initialize result matrix to zero
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i * cols + j] = 0;
        }
    }
    
    // Perform matrix multiplication
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < common; k++) {
                C[i * cols + j] += A[i * common + k] * B[k * cols + j];
            }
        }
    }
}

/**
 * Matrix multiplication function with 2D array syntax
 * 
 * @param A First matrix (rows x common)
 * @param B Second matrix (common x cols)
 * @param C Result matrix (rows x cols)
 * @param rows Number of rows in matrix A and C
 * @param cols Number of columns in matrix B and C
 * @param common Common dimension (columns of A, rows of B)
 */
void matrixMultiply2D(int A[][10], int B[][10], int C[][10], int rows, int cols, int common) {
    // Initialize result matrix to zero
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i][j] = 0;
        }
    }
    
    // Perform matrix multiplication
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < common; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

/**
 * Matrix multiplication function with fixed dimensions
 * 
 * @param A First matrix (3x4)
 * @param B Second matrix (4x2)
 * @param C Result matrix (3x2)
 */
void matrixMultiplyFixed(int A[3][4], int B[4][2], int C[3][2]) {
    // Initialize result matrix to zero
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            C[i][j] = 0;
        }
    }
    
    // Perform matrix multiplication
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    // Example matrices
    int A[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    
    int B[3][2] = {
        {7, 8},
        {9, 10},
        {11, 12}
    };
    
    int C[2][2] = {0}; // Result matrix
    
    // Call matrix multiplication function
    matrixMultiplyFixed(A, B, C);
    
    // Print result
    std::cout << "Result matrix:" << std::endl;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
