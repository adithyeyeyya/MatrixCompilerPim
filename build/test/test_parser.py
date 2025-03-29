#!/usr/bin/env python3
"""
Test script for the Parser component of the PIM compiler
"""

import os
import sys
import subprocess
import tempfile
import unittest

class ParserTest(unittest.TestCase):
    
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
    
    def test_basic_matrix_multiplication(self):
        """Test parsing of a basic matrix multiplication program"""
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_basic.cpp")
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
        
        # Run the compiler with --dump-ir to get output without generating PIM instructions
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "--dump-ir", "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if parsing was successful
        self.assertEqual(result.returncode, 0, 
                         f"Parser test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for expected log messages
        self.assertIn("Parsing input file", result.stdout)
        self.assertIn("Translation unit parsed successfully", result.stderr)
    
    def test_parameterized_matrix_multiplication(self):
        """Test parsing of a parameterized matrix multiplication program"""
        
        # Create a test C++ file with parameterized sizes
        test_file = os.path.join(self.temp_dir.name, "test_parameterized.cpp")
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
        
        # Run the compiler with --dump-ir to get output without generating PIM instructions
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "--dump-ir", "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if parsing was successful
        self.assertEqual(result.returncode, 0, 
                         f"Parser test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for expected log messages
        self.assertIn("Parsing input file", result.stdout)
        self.assertIn("Translation unit parsed successfully", result.stderr)
    
    def test_syntax_error(self):
        """Test handling of syntax errors in the input file"""
        
        # Create a test C++ file with syntax errors
        test_file = os.path.join(self.temp_dir.name, "test_error.cpp")
        with open(test_file, "w") as f:
            f.write("""
            void matrixMultiply( // Missing closing parenthesis
                for (int i = 0; i < 10; i++) {
                    // Missing semicolon
                    int x = 5
                }
            }
            """)
        
        # Run the compiler
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if parser correctly detected the syntax error
        self.assertNotEqual(result.returncode, 0, "Parser should have detected syntax errors")
        self.assertIn("Error:", result.stderr)

if __name__ == "__main__":
    unittest.main()
