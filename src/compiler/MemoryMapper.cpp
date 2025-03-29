/**
 * MemoryMapper.cpp
 * Implements memory access transformation for PIM architecture
 */

#include "MemoryMapper.h"
#include "../utils/Logger.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <map>
#include <string>
#include <vector>

MemoryMapper::MemoryMapper() {}

MemoryMapper::~MemoryMapper() = default;

std::unique_ptr<llvm::Module> MemoryMapper::applyMemoryMapping(std::unique_ptr<llvm::Module>& module) {
    Logger::getInstance().log("Starting memory mapping transformation");
    
    // Detect matrix dimensions from the code
    auto matrixInfo = detectMatrixDimensions(module.get());
    
    // Process each function in the module
    for (auto& function : *module) {
        // Skip declarations without definitions
        if (function.isDeclaration()) {
            continue;
        }
        
        Logger::getInstance().log("Applying memory mapping to function: " + function.getName().str());
        mapArrayAccesses(&function, matrixInfo);
    }
    
    Logger::getInstance().log("Memory mapping transformation complete");
    return std::move(module);
}

std::map<std::string, std::vector<unsigned>> MemoryMapper::detectMatrixDimensions(llvm::Module* module) {
    Logger::getInstance().log("Detecting matrix dimensions");
    
    // In a real implementation, this would analyze the code to determine matrix dimensions
    // For simplicity, we'll use fixed dimensions in this example
    std::map<std::string, std::vector<unsigned>> matrixInfo;
    
    // Add default matrix information
    // Format: {rows, cols}
    matrixInfo["A"] = {2, 2};
    matrixInfo["B"] = {2, 2};
    matrixInfo["C"] = {2, 2};
    
    return matrixInfo;
}

void MemoryMapper::mapArrayAccesses(llvm::Function* function, std::map<std::string, std::vector<unsigned>>& matrixInfo) {
    std::vector<llvm::Instruction*> instructionsToProcess;
    
    // First, collect all load/store instructions
    for (auto& block : *function) {
        for (auto& inst : block) {
            if (llvm::isa<llvm::LoadInst>(inst) || llvm::isa<llvm::StoreInst>(inst)) {
                instructionsToProcess.push_back(&inst);
            }
        }
    }
    
    // Then transform them
    for (auto* inst : instructionsToProcess) {
        transformMemoryAccess(inst, matrixInfo);
    }
}

bool MemoryMapper::transformMemoryAccess(llvm::Instruction* inst, std::map<std::string, std::vector<unsigned>>& matrixInfo) {
    llvm::IRBuilder<> builder(inst);
    llvm::Value* ptr = nullptr;
    
    // Get pointer operand based on instruction type
    if (auto* loadInst = llvm::dyn_cast<llvm::LoadInst>(inst)) {
        ptr = loadInst->getPointerOperand();
    } else if (auto* storeInst = llvm::dyn_cast<llvm::StoreInst>(inst)) {
        ptr = storeInst->getPointerOperand();
    } else {
        return false;
    }
    
    // Check if this is an array access
    auto* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr);
    if (!gep) {
        return false;
    }
    
    // In a real implementation, we would analyze the GEP to determine which matrix it's accessing
    // and transform the access according to PIM memory layout
    
    // Determine which matrix is being accessed
    llvm::Value* basePtr = gep->getPointerOperand();
    std::string matrixName;
    
    if (auto* global = llvm::dyn_cast<llvm::GlobalVariable>(basePtr)) {
        matrixName = global->getName().str();
    } else {
        // We can't determine the matrix name, skip this instruction
        return false;
    }
    
    // Check if we have information about this matrix
    if (matrixInfo.find(matrixName) == matrixInfo.end()) {
        return false;
    }
    
    // Get the indices
    if (gep->getNumIndices() != 2) {
        // Not a 2D array access, skip
        return false;
    }
    
    llvm::Value* rowIdx = gep->getOperand(1);
    llvm::Value* colIdx = gep->getOperand(2);
    
    // Check if indices are constants
    auto* rowIdxConst = llvm::dyn_cast<llvm::ConstantInt>(rowIdx);
    auto* colIdxConst = llvm::dyn_cast<llvm::ConstantInt>(colIdx);
    
    if (!rowIdxConst || !colIdxConst) {
        // Indices are not constants, we would need more complex transformation
        // For simplicity, we'll skip these cases
        return false;
    }
    
    // Get matrix dimensions
    unsigned rows = matrixInfo[matrixName][0];
    unsigned cols = matrixInfo[matrixName][1];
    
    // Calculate linear index based on PIM memory layout
    // In a real implementation, this would follow the PIM memory mapping scheme
    // For simplicity, we'll use row-major order
    unsigned rowVal = rowIdxConst->getZExtValue();
    unsigned colVal = colIdxConst->getZExtValue();
    unsigned linearIdx = rowVal * cols + colVal;
    
    // Create a new GEP instruction that uses the linear index
    std::vector<llvm::Value*> indices;
    indices.push_back(llvm::ConstantInt::get(rowIdx->getType(), 0));  // First index is always 0 for arrays
    indices.push_back(llvm::ConstantInt::get(rowIdx->getType(), linearIdx));
    
    llvm::Value* newPtr = builder.CreateGEP(gep->getSourceElementType(), basePtr, indices, "pim_" + matrixName + "_addr");
    
    // Replace the old pointer with the new one
    if (auto* loadInst = llvm::dyn_cast<llvm::LoadInst>(inst)) {
        loadInst->setOperand(0, newPtr);
    } else if (auto* storeInst = llvm::dyn_cast<llvm::StoreInst>(inst)) {
        storeInst->setOperand(1, newPtr);
    }
    
    return true;
}
