#!/usr/bin/env python3
"""
Test script for the Memory Mapper component of the PIM compiler
"""

import os
import sys
import subprocess
import tempfile
import unittest

class MemoryMapperTest(unittest.TestCase):
    
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
    
    def test_memory_mapping_basic(self):
        """Test basic memory mapping for matrix multiplication"""
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_memory_basic.cpp")
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
        
        # Run the compiler with verbose output to check memory mapping
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if compilation was successful
        self.assertEqual(result.returncode, 0, 
                         f"Memory mapper test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for memory mapping logs
        self.assertIn("memory mapping", result.stdout.lower())
        
        # Check output file for expected memory-related instructions
        with open(output_file, "r") as f:
            content = f.read()
            self.assertIn("LOAD", content)
            self.assertIn("STORE", content)
    
    def test_memory_mapping_linear_arrays(self):
        """Test memory mapping for linear arrays"""
        
        # Create a test C++ file using linear arrays
        test_file = os.path.join(self.temp_dir.name, "test_memory_linear.cpp")
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
        
        # Run the compiler with verbose output
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if compilation was successful
        self.assertEqual(result.returncode, 0, 
                         f"Memory mapper test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for memory mapping logs
        self.assertIn("memory mapping", result.stdout.lower())
        
        # Check output file for expected memory-related instructions
        with open(output_file, "r") as f:
            content = f.read()
            self.assertIn("LOAD", content)
            self.assertIn("STORE", content)
    
    def test_memory_mapping_disabled(self):
        """Test compilation with memory mapping disabled"""
        
        # TODO: Add an option to disable memory mapping in the compiler
        # This is a placeholder test for now
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_memory_disabled.cpp")
        with open(test_file, "w") as f:
            f.write("""
            void matrixMultiply(int A[][2], int B[][2], int C[][2]) {
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        C[i][j] = 0;
                        for (int k = 0; k < 2; k++) {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
            """)
        
        # Run the compiler with memory mapping (since we can't disable it yet)
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if compilation was successful
        self.assertEqual(result.returncode, 0, 
                         f"Memory mapper test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check for memory mapping logs
        self.assertIn("memory mapping", result.stdout.lower())

if __name__ == "__main__":
    unittest.main()
