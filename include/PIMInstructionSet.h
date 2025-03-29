/**
 * PIMInstructionSet.h
 * Defines the instruction set architecture for the PIM system
 * Based on Section IV-D of the referenced paper
 */

#ifndef PIM_INSTRUCTION_SET_H
#define PIM_INSTRUCTION_SET_H

#include <cstdint>

/**
 * PIM Opcodes
 * Defines the operation codes for the PIM instruction set
 */
enum PIMOpcode {
    PIM_NOP = 0,     // No operation
    PIM_LOAD,        // Load data from host memory to PIM memory
    PIM_STORE,       // Store data from PIM memory to host memory
    PIM_MOVE,        // Move data between PIM registers or memory
    PIM_ADD,         // Integer addition
    PIM_SUB,         // Integer subtraction
    PIM_MUL,         // Integer multiplication
    PIM_DIV,         // Integer division
    PIM_AND,         // Bitwise AND
    PIM_OR,          // Bitwise OR
    PIM_XOR,         // Bitwise XOR
    PIM_NOT,         // Bitwise NOT
    PIM_SHL,         // Shift left
    PIM_SHR,         // Shift right
    PIM_JUMP,        // Unconditional jump
    PIM_JUMPZ,       // Jump if zero
    PIM_JUMPNZ,      // Jump if not zero
    PIM_CONFIG       // Configure PIM parameters
};

/**
 * PIM Register IDs
 * Defines the registers available in the PIM architecture
 */
enum PIMRegister {
    PIM_REG0 = 0,    // General purpose register 0
    PIM_REG1,        // General purpose register 1
    PIM_REG2,        // General purpose register 2
    PIM_REG3,        // General purpose register 3
    PIM_REG4,        // General purpose register 4
    PIM_REG5,        // General purpose register 5
    PIM_REG6,        // General purpose register 6
    PIM_REG7,        // General purpose register 7
    PIM_PC,          // Program counter
    PIM_STATUS       // Status register
};

/**
 * PIM Configuration Parameters
 * Parameters that can be configured with the CONFIG instruction
 */
enum PIMConfigParam {
    PIM_CONFIG_ARRAY_SIZE = 0,    // Size of the PIM processing array
    PIM_CONFIG_OP_MODE,           // Operation mode
    PIM_CONFIG_PRECISION,         // Precision (e.g., 8-bit, 16-bit, 32-bit)
    PIM_CONFIG_INTERCONNECT       // Interconnect configuration
};

/**
 * PIM Memory Layout
 * Defines the memory organization within the PIM architecture
 */
struct PIMMemoryLayout {
    static const uint32_t MATRIX_A_OFFSET = 0;                // Starting offset for Matrix A
    static const uint32_t MATRIX_B_OFFSET = 1024;             // Starting offset for Matrix B
    static const uint32_t MATRIX_C_OFFSET = 2048;             // Starting offset for Matrix C (result)
    static const uint32_t REGISTER_FILE_OFFSET = 4096;        // Starting offset for register file
    static const uint32_t INSTRUCTION_MEMORY_OFFSET = 5120;   // Starting offset for instruction memory
};

/**
 * PIM Instruction Format
 * 
 * 32-bit instruction format:
 * [31:26] - Opcode (6 bits)
 * [25:18] - Destination (8 bits)
 * [17:10] - Source 1 (8 bits)
 * [9:2]   - Source 2 (8 bits)
 * [1:0]   - Immediate (2 bits)
 */
struct PIMInstructionFormat {
    static const uint32_t OPCODE_SHIFT = 26;
    static const uint32_t OPCODE_MASK = 0x3F;  // 6 bits
    
    static const uint32_t DEST_SHIFT = 18;
    static const uint32_t DEST_MASK = 0xFF;    // 8 bits
    
    static const uint32_t SRC1_SHIFT = 10;
    static const uint32_t SRC1_MASK = 0xFF;    // 8 bits
    
    static const uint32_t SRC2_SHIFT = 2;
    static const uint32_t SRC2_MASK = 0xFF;    // 8 bits
    
    static const uint32_t IMM_SHIFT = 0;
    static const uint32_t IMM_MASK = 0x3;      // 2 bits
    
    static uint32_t encode(PIMOpcode opcode, uint32_t dest, uint32_t src1, uint32_t src2, uint32_t imm) {
        return ((opcode & OPCODE_MASK) << OPCODE_SHIFT) |
               ((dest & DEST_MASK) << DEST_SHIFT) |
               ((src1 & SRC1_MASK) << SRC1_SHIFT) |
               ((src2 & SRC2_MASK) << SRC2_SHIFT) |
               ((imm & IMM_MASK) << IMM_SHIFT);
    }
    
    static PIMOpcode decodeOpcode(uint32_t instruction) {
        return static_cast<PIMOpcode>((instruction >> OPCODE_SHIFT) & OPCODE_MASK);
    }
    
    static uint32_t decodeDest(uint32_t instruction) {
        return (instruction >> DEST_SHIFT) & DEST_MASK;
    }
    
    static uint32_t decodeSrc1(uint32_t instruction) {
        return (instruction >> SRC1_SHIFT) & SRC1_MASK;
    }
    
    static uint32_t decodeSrc2(uint32_t instruction) {
        return (instruction >> SRC2_SHIFT) & SRC2_MASK;
    }
    
    static uint32_t decodeImm(uint32_t instruction) {
        return (instruction >> IMM_SHIFT) & IMM_MASK;
    }
};

#endif // PIM_INSTRUCTION_SET_H
