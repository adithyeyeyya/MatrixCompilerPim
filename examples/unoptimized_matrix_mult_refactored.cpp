/**
 * unoptimized_matrix_mult.cpp
 * Unoptimized example C++ program for matrix multiplication to be refactored by the PIM compiler
 */

#include <iostream>

/**
 * Simple unoptimized matrix multiplication function
 * 
 * This function has a suboptimal loop ordering (i-j-k) and inefficient memory access patterns
 * that could be improved for better performance on PIM architecture.
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
    
    // Perform matrix multiplication with suboptimal loop ordering (i-j-k)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < common; k++) {
                C[i * cols + j] += A[i * common + k] * B[k * cols + j];
            }
        }
    }
}

/**
 * Matrix multiplication function with 2D array syntax and suboptimal implementation
 */
void matrixMultiply2D(int A[][64], int B[][64], int C[][64], int rows, int cols, int common) {
    // Initialize result matrix to zero
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i][j] = 0;
        }
    }
    
    // Perform matrix multiplication with suboptimal loop ordering (i-j-k)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < common; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

/**
 * Matrix multiplication for fixed-size matrices with inefficient memory access
 */
void matrixMultiplyBlocked(int* A, int* B, int* C, int rows, int cols, int common) {
    const int BLOCK_SIZE = 8; // Optimal block size for PIM architecture

    // Initialize result matrix to zero
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            C[i][j] = 0;
        }
    }
    
    // Perform matrix multiplication without blocking/tiling
    // Use blocked matrix multiplication algorithm
    for (int i = 0; i < rows; i += BLOCK_SIZE) {
        for (int j = 0; j < cols; j += BLOCK_SIZE) {
            for (int k = 0; k < common; k += BLOCK_SIZE) {
                // Process block
                for (int ii = i; ii < std::min(i + BLOCK_SIZE, rows); ii++) {
                    for (int jj = j; jj < std::min(j + BLOCK_SIZE, cols); jj++) {
                        for (int kk = k; kk < std::min(k + BLOCK_SIZE, common); kk++) {
                C[ii][jj] += A[ii][kk] * B[kk][jj];
            }
        }
    }
                }
            }
        }
}

int main() {
    // Initialize small test matrices
    int A[2][2] = {
        {1, 2},
        {3, 4}
    };
    
    int B[2][2] = {
        {5, 6},
        {7, 8}
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