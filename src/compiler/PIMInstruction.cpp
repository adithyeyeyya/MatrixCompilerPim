/**
 * PIMInstruction.cpp
 * Implementation of PIM instruction class
 */

#include "PIMInstruction.h"
#include <sstream>
#include <iomanip>

PIMInstruction::PIMInstruction(PIMOpcode opcode, unsigned dest, unsigned src1, unsigned src2, unsigned imm)
    : opcode(opcode), dest(dest), src1(src1), src2(src2), imm(imm) {}

PIMInstruction::PIMInstruction(const PIMInstruction& other)
    : opcode(other.opcode), dest(other.dest), src1(other.src1), src2(other.src2), imm(other.imm) {}

PIMOpcode PIMInstruction::getOpcode() const {
    return opcode;
}

unsigned PIMInstruction::getDest() const {
    return dest;
}

unsigned PIMInstruction::getSrc1() const {
    return src1;
}

unsigned PIMInstruction::getSrc2() const {
    return src2;
}

unsigned PIMInstruction::getImm() const {
    return imm;
}

uint32_t PIMInstruction::toBinary() const {
    // Format based on PIM ISA specification
    // Assuming:
    // - opcode: 6 bits
    // - dest: 8 bits
    // - src1: 8 bits
    // - src2: 8 bits
    // - imm: 2 bits (as described in the paper, though this seems very small)
    
    uint32_t result = 0;
    result |= (static_cast<uint32_t>(opcode) & 0x3F) << 26;  // 6-bit opcode in the highest bits
    result |= (dest & 0xFF) << 18;                           // 8-bit dest
    result |= (src1 & 0xFF) << 10;                           // 8-bit src1
    result |= (src2 & 0xFF) << 2;                            // 8-bit src2
    result |= (imm & 0x3);                                   // 2-bit immediate
    
    return result;
}

std::string PIMInstruction::toString() const {
    std::stringstream ss;
    
    // Add the opcode name
    switch (opcode) {
        case PIM_NOP:     ss << "NOP"; break;
        case PIM_LOAD:    ss << "LOAD"; break;
        case PIM_STORE:   ss << "STORE"; break;
        case PIM_MOVE:    ss << "MOVE"; break;
        case PIM_ADD:     ss << "ADD"; break;
        case PIM_SUB:     ss << "SUB"; break;
        case PIM_MUL:     ss << "MUL"; break;
        case PIM_DIV:     ss << "DIV"; break;
        case PIM_AND:     ss << "AND"; break;
        case PIM_OR:      ss << "OR"; break;
        case PIM_XOR:     ss << "XOR"; break;
        case PIM_NOT:     ss << "NOT"; break;
        case PIM_SHL:     ss << "SHL"; break;
        case PIM_SHR:     ss << "SHR"; break;
        case PIM_JUMP:    ss << "JUMP"; break;
        case PIM_JUMPZ:   ss << "JUMPZ"; break;
        case PIM_JUMPNZ:  ss << "JUMPNZ"; break;
        case PIM_CONFIG:  ss << "CONFIG"; break;
        default:          ss << "UNKNOWN"; break;
    }
    
    // Format depends on instruction type
    if (opcode == PIM_NOP) {
        // NOP has no operands
    } else if (opcode == PIM_CONFIG) {
        // CONFIG has a parameter ID and value
        ss << " " << dest << ", " << src1;
    } else if (opcode == PIM_LOAD || opcode == PIM_STORE) {
        // Load/Store have memory addresses and optional row/column
        ss << " " << dest << ", " << src1;
        if (src2 != 0 || imm != 0) {
            ss << " [" << src2 << ", " << imm << "]";
        }
    } else if (opcode == PIM_NOT || opcode == PIM_JUMP) {
        // Single operand instructions
        ss << " " << dest;
    } else if (opcode == PIM_JUMPZ || opcode == PIM_JUMPNZ) {
        // Conditional jumps
        ss << " " << dest << ", " << src1;
    } else {
        // Regular 3-operand instructions
        ss << " " << dest << ", " << src1 << ", " << src2;
        if (imm != 0) {
            ss << ", " << imm;
        }
    }
    
    // Add the binary representation
    ss << " ; 0x" << std::hex << std::setw(8) << std::setfill('0') << toBinary();
    
    return ss.str();
}
