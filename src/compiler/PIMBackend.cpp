/**
 * PIMBackend.cpp
 * Implements translation from LLVM IR to PIM ISA instructions
 */

#include "PIMBackend.h"
#include "../utils/Logger.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/raw_ostream.h>

PIMBackend::PIMBackend() {}

PIMBackend::~PIMBackend() = default;

std::vector<PIMInstruction> PIMBackend::generatePIMInstructions(std::unique_ptr<llvm::Module>& module) {
    Logger::getInstance().log("Starting PIM instruction generation");
    
    std::vector<PIMInstruction> instructions;
    
    // Process each function in the module
    for (auto& function : module->functions()) {
        // Skip declarations without definitions
        if (function.isDeclaration()) {
            continue;
        }
        
        Logger::getInstance().log("Processing function: " + function.getName().str());
        processMatrixMultiplyFunction(&function, instructions);
    }
    
    Logger::getInstance().log("Generated " + std::to_string(instructions.size()) + " PIM instructions");
    return instructions;
}

void PIMBackend::processMatrixMultiplyFunction(llvm::Function* function, std::vector<PIMInstruction>& instructions) {
    // Analyze the function to determine matrix dimensions
    // In a real implementation, we would extract this from the IR
    // For simplicity, we're using fixed dimensions in this example
    
    unsigned rows = 2;
    unsigned cols = 2;
    unsigned common = 2;
    
    Logger::getInstance().log("Matrix dimensions: " + std::to_string(rows) + "x" + 
                             std::to_string(common) + " * " + std::to_string(common) + "x" + 
                             std::to_string(cols));
    
    // Generate instructions for each phase of matrix multiplication
    
    // 1. Load matrices into PIM memory
    generateMatrixLoadInstructions(instructions, rows, cols, common);
    
    // 2. Perform matrix multiplication
    generateMatrixMultiplyInstructions(instructions, rows, cols, common);
    
    // 3. Store result
    generateStoreResultInstructions(instructions, rows, cols);
}

void PIMBackend::generateMatrixLoadInstructions(std::vector<PIMInstruction>& instructions, 
                                               unsigned rows, unsigned cols, unsigned common) {
    Logger::getInstance().log("Generating matrix load instructions");
    
    // Configure the PIM array size
    instructions.push_back(PIMInstruction(PIM_CONFIG, 0, rows*common, 0, 0));
    instructions.push_back(PIMInstruction(PIM_CONFIG, 1, common*cols, 0, 0));
    instructions.push_back(PIMInstruction(PIM_CONFIG, 2, rows*cols, 0, 0));
    
    // Load matrix A (rows x common)
    for (unsigned i = 0; i < rows; i++) {
        for (unsigned k = 0; k < common; k++) {
            // LOAD instruction: opcode = PIM_LOAD, dest = matrix A offset + i*common + k, src = host memory address
            instructions.push_back(PIMInstruction(PIM_LOAD, 
                                                 (i * common + k),             // destination PIM address
                                                 0,                            // src (host memory offset, would be actual address)
                                                 i,                            // row
                                                 k));                          // col
        }
    }
    
    // Load matrix B (common x cols)
    for (unsigned k = 0; k < common; k++) {
        for (unsigned j = 0; j < cols; j++) {
            // LOAD instruction: opcode = PIM_LOAD, dest = matrix B offset + k*cols + j, src = host memory address
            instructions.push_back(PIMInstruction(PIM_LOAD, 
                                                 rows*common + (k * cols + j), // destination PIM address
                                                 0,                            // src (host memory offset, would be actual address)
                                                 k,                            // row
                                                 j));                          // col
        }
    }
    
    // Initialize matrix C (rows x cols) to zeros
    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            // LOAD instruction with zero value: opcode = PIM_LOAD, dest = matrix C offset + i*cols + j, src = 0
            instructions.push_back(PIMInstruction(PIM_LOAD, 
                                                 rows*common + common*cols + (i * cols + j), // destination PIM address
                                                 0,                                          // src (zero)
                                                 i,                                          // row
                                                 j));                                        // col
        }
    }
}

void PIMBackend::generateMatrixMultiplyInstructions(std::vector<PIMInstruction>& instructions,
                                                   unsigned rows, unsigned cols, unsigned common) {
    Logger::getInstance().log("Generating matrix multiply instructions");
    
    // The PIM-specific way to do matrix multiplication
    // In a real PIM architecture, we'd use specialized matrix operations
    
    // Calculate C = A * B
    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            for (unsigned k = 0; k < common; k++) {
                // Address calculations
                unsigned a_addr = i * common + k;
                unsigned b_addr = rows*common + k * cols + j;
                unsigned c_addr = rows*common + common*cols + i * cols + j;
                
                // Load values from A and B into registers
                instructions.push_back(PIMInstruction(PIM_MOVE, 0, a_addr, 0, 0));   // Move A[i][k] to Reg0
                instructions.push_back(PIMInstruction(PIM_MOVE, 1, b_addr, 0, 0));   // Move B[k][j] to Reg1
                
                // Multiply A[i][k] * B[k][j]
                instructions.push_back(PIMInstruction(PIM_MUL, 2, 0, 1, 0));         // Reg2 = Reg0 * Reg1
                
                // Add to C[i][j]
                instructions.push_back(PIMInstruction(PIM_MOVE, 3, c_addr, 0, 0));   // Move C[i][j] to Reg3
                instructions.push_back(PIMInstruction(PIM_ADD, 3, 3, 2, 0));         // Reg3 = Reg3 + Reg2
                
                // Store result back to C[i][j]
                instructions.push_back(PIMInstruction(PIM_MOVE, c_addr, 3, 0, 0));   // Move Reg3 to C[i][j]
            }
        }
    }
}

void PIMBackend::generateStoreResultInstructions(std::vector<PIMInstruction>& instructions,
                                                unsigned rows, unsigned cols) {
    Logger::getInstance().log("Generating store result instructions");
    
    // Store matrix C (rows x cols) back to host memory
    unsigned c_offset = rows*cols + cols*cols;
    
    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            // Get address of C[i][j]
            unsigned c_addr = rows*cols + cols*cols + i * cols + j;
            
            // STORE instruction: opcode = PIM_STORE, dest = host memory address, src = C[i][j] PIM address
            instructions.push_back(PIMInstruction(PIM_STORE, 
                                                 0,                // destination (host memory offset, would be actual address)
                                                 c_addr,           // src PIM address
                                                 i,                // row
                                                 j));              // col
        }
    }
}
