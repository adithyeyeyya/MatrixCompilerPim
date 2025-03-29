#!/usr/bin/env python3
"""
Test script for the IR Generation component of the PIM compiler
"""

import os
import sys
import subprocess
import tempfile
import unittest

class IRGenerationTest(unittest.TestCase):
    
    def setUp(self):
        # Path to the compiler executable
        self.compiler_path = os.path.join("..", "build", "pim_compiler")
        
        # Check if the compiler exists
        if not os.path.exists(self.compiler_path):
            self.skipTest("Compiler executable not found. Build the project first.")
        
        # Create a temporary directory for test files
        self.temp_dir = tempfile.TemporaryDirectory()
    
    def tearDown(self):
        # Clean up the temporary directory
        self.temp_dir.cleanup()
    
    def test_ir_generation_basic(self):
        """Test LLVM IR generation for a basic matrix multiplication"""
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_ir_basic.cpp")
        with open(test_file, "w") as f:
            f.write("""
            void matrixMultiply(int A[][10], int B[][10], int C[][10], int m, int n, int p) {
                for (int i = 0; i < m; i++) {
                    for (int j = 0; j < p; j++) {
                        C[i][j] = 0;
                        for (int k = 0; k < n; k++) {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
            """)
        
        # Run the compiler with --dump-ir to check IR generation
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "--dump-ir", "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if IR generation was successful
        self.assertEqual(result.returncode, 0, 
                         f"IR generation test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for expected IR elements in the output
        self.assertIn("define", result.stderr)
        self.assertIn("matrixMultiply", result.stderr)
        self.assertIn("alloca", result.stderr)
        self.assertIn("store", result.stderr)
        self.assertIn("load", result.stderr)
        self.assertIn("mul", result.stderr)
        self.assertIn("add", result.stderr)
    
    def test_ir_generation_parameterized(self):
        """Test LLVM IR generation for parameterized matrix multiplication"""
        
        # Create a test C++ file with parameterized dimensions
        test_file = os.path.join(self.temp_dir.name, "test_ir_param.cpp")
        with open(test_file, "w") as f:
            f.write("""
            void matrixMultiply(int* A, int* B, int* C, int rows, int cols, int common) {
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        C[i * cols + j] = 0;
                        for (int k = 0; k < common; k++) {
                            C[i * cols + j] += A[i * common + k] * B[k * cols + j];
                        }
                    }
                }
            }
            """)
        
        # Run the compiler with --dump-ir to check IR generation
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "--dump-ir", "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if IR generation was successful
        self.assertEqual(result.returncode, 0, 
                         f"IR generation test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for expected IR elements in the output
        self.assertIn("define", result.stderr)
        self.assertIn("matrixMultiply", result.stderr)
        self.assertIn("getelementptr", result.stderr)
    
    def test_ir_generation_complex_expressions(self):
        """Test LLVM IR generation for complex array indexing expressions"""
        
        # Create a test C++ file with complex indexing
        test_file = os.path.join(self.temp_dir.name, "test_ir_complex.cpp")
        with open(test_file, "w") as f:
            f.write("""
            void matrixMultiply(int* A, int* B, int* C, int rows, int cols, int common) {
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        C[i * cols + j] = 0;
                        for (int k = 0; k < common; k++) {
                            // Complex indexing expressions
                            int a_idx = i * common + k;
                            int b_idx = k * cols + j;
                            int c_idx = i * cols + j;
                            
                            // Complex computation
                            C[c_idx] += (A[a_idx] * 2) * (B[b_idx] / 2);
                        }
                    }
                }
            }
            """)
        
        # Run the compiler with --dump-ir to check IR generation
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "--dump-ir", "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if IR generation was successful
        self.assertEqual(result.returncode, 0, 
                         f"IR generation test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for expected IR elements in the output
        self.assertIn("define", result.stderr)
        self.assertIn("matrixMultiply", result.stderr)
        self.assertIn("mul", result.stderr)
        self.assertIn("sdiv", result.stderr)  # For division

if __name__ == "__main__":
    unittest.main()
