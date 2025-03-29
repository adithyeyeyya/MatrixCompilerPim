/**
 * MemoryMapper.h
 * Maps standard memory accesses to PIM architecture memory layout
 */

#ifndef MEMORY_MAPPER_H
#define MEMORY_MAPPER_H

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

class MemoryMapper {
public:
    MemoryMapper();
    ~MemoryMapper();

    /**
     * Apply memory mapping to the LLVM IR for PIM architecture
     * 
     * @param module LLVM module to transform
     * @return Transformed LLVM module
     */
    std::unique_ptr<llvm::Module> applyMemoryMapping(std::unique_ptr<llvm::Module>& module);

private:
    /**
     * Map array accesses to PIM-specific memory layout
     * 
     * @param function Function to transform
     * @param matrixInfo Map of matrix names to dimensions
     */
    void mapArrayAccesses(llvm::Function* function, std::map<std::string, std::vector<unsigned>>& matrixInfo);
    
    /**
     * Detect matrix dimensions from the code
     * 
     * @param module LLVM module to analyze
     * @return Map of matrix names to dimensions
     */
    std::map<std::string, std::vector<unsigned>> detectMatrixDimensions(llvm::Module* module);
    
    /**
     * Transform regular load/store instructions to PIM-specific layout
     * 
     * @param inst Load or store instruction to transform
     * @param matrixInfo Map of matrix names to dimensions
     * @return Whether the instruction was transformed
     */
    bool transformMemoryAccess(llvm::Instruction* inst, std::map<std::string, std::vector<unsigned>>& matrixInfo);
};

#endif // MEMORY_MAPPER_H
