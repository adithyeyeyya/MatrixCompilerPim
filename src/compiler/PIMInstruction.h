/**
 * PIMInstruction.h
 * Represents a single instruction for the PIM architecture
 */

#ifndef PIM_INSTRUCTION_H
#define PIM_INSTRUCTION_H

#include <string>
#include "../include/PIMInstructionSet.h"

class PIMInstruction {
public:
    // Constructor
    PIMInstruction(PIMOpcode opcode, unsigned dest, unsigned src1, unsigned src2, unsigned imm);
    
    // Copy constructor
    PIMInstruction(const PIMInstruction& other);
    
    // Accessors
    PIMOpcode getOpcode() const;
    unsigned getDest() const;
    unsigned getSrc1() const;
    unsigned getSrc2() const;
    unsigned getImm() const;
    
    // Convert instruction to binary format
    uint32_t toBinary() const;
    
    // Convert instruction to string representation
    std::string toString() const;

private:
    PIMOpcode opcode; // Operation code
    unsigned dest;    // Destination register/memory address
    unsigned src1;    // Source operand 1
    unsigned src2;    // Source operand 2
    unsigned imm;     // Immediate value
};

#endif // PIM_INSTRUCTION_H
