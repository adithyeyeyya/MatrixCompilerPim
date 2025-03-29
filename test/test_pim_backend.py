#!/usr/bin/env python3
"""
Test script for the PIM Backend component of the PIM compiler
"""

import os
import sys
import subprocess
import tempfile
import unittest
import re

class PIMBackendTest(unittest.TestCase):
    
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
    
    def test_pim_instruction_generation(self):
        """Test generation of PIM instructions for matrix multiplication"""
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_pim_gen.cpp")
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
        
        # Run the compiler to generate PIM instructions
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if instruction generation was successful
        self.assertEqual(result.returncode, 0, 
                         f"PIM backend test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check that the output file exists and contains PIM instructions
        self.assertTrue(os.path.exists(output_file), "Output file was not created")
        
        with open(output_file, "r") as f:
            content = f.read()
            
            # Check for expected instruction types
            self.assertIn("CONFIG", content)
            self.assertIn("LOAD", content)
            self.assertIn("MOVE", content)
            self.assertIn("MUL", content)
            self.assertIn("ADD", content)
            self.assertIn("STORE", content)
            
            # Count instructions
            instruction_count = len(content.strip().split("\n"))
            self.assertGreater(instruction_count, 10, "Too few instructions generated")
    
    def test_pim_instruction_format(self):
        """Test the format of generated PIM instructions"""
        
        # Create a test C++ file
        test_file = os.path.join(self.temp_dir.name, "test_pim_format.cpp")
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
        
        # Run the compiler to generate PIM instructions
        output_file = os.path.join(self.temp_dir.name, "output.txt")
        result = subprocess.run(
            [self.compiler_path, "-v", "-o", output_file, test_file],
            capture_output=True,
            text=True
        )
        
        # Check if instruction generation was successful
        self.assertEqual(result.returncode, 0, 
                         f"PIM backend test failed with return code {result.returncode}. Error: {result.stderr}")
        
        # Check the format of instructions
        with open(output_file, "r") as f:
            content = f.read()
            lines = content.strip().split("\n")
            
            for line in lines:
                # Check the instruction format - should match our ISA format
                # Most instructions should have format: OPCODE DEST, SRC1, SRC2 ; 0xHEXVALUE
                if not line.startswith("NOP"):  # NOP has no operands
                    parts = line.split(" ; ")
                    self.assertEqual(len(parts), 2, f"Instruction missing binary representation: {line}")
                    
                    # Check binary representation format
                    binary_part = parts[1]
                    self.assertTrue(binary_part.startswith("0x"), 
                                   f"Binary representation doesn't start with 0x: {binary_part}")
                    self.assertTrue(re.match(r"0x[0-9a-fA-F]{8}", binary_part), 
                                   f"Binary representation not in 32-bit hex format: {binary_part}")
    
    def test_matrix_size_handling(self):
        """Test handling of different matrix sizes"""
        
        # Create test files with different matrix sizes
        sizes = [(2, 2, 2), (3, 4, 2), (10, 10, 10)]
        
        for size in sizes:
            rows, common, cols = size
            
            # Create a test C++ file
            test_file = os.path.join(self.temp_dir.name, f"test_size_{rows}_{common}_{cols}.cpp")
            with open(test_file, "w") as f:
                f.write(f"""
                void matrixMultiply(int* A, int* B, int* C) {{
                    // Matrix A: {rows}x{common}, Matrix B: {common}x{cols}, Matrix C: {rows}x{cols}
                    for (int i = 0; i < {rows}; i++) {{
                        for (int j = 0; j < {cols}; j++) {{
                            C[i * {cols} + j] = 0;
                            for (int k = 0; k < {common}; k++) {{
                                C[i * {cols} + j] += A[i * {common} + k] * B[k * {cols} + j];
                            }}
                        }}
                    }}
                }}
                """)
            
            # Run the compiler to generate PIM instructions
            output_file = os.path.join(self.temp_dir.name, f"output_{rows}_{common}_{cols}.txt")
            result = subprocess.run(
                [self.compiler_path, "-v", "-o", output_file, test_file],
                capture_output=True,
                text=True
            )
            
            # Check if instruction generation was successful
            self.assertEqual(result.returncode, 0, 
                            f"PIM backend test for size {size} failed with return code {result.returncode}. Error: {result.stderr}")
            
            # Check that the output file exists
            self.assertTrue(os.path.exists(output_file), f"Output file for size {size} was not created")

if __name__ == "__main__":
    unittest.main()
