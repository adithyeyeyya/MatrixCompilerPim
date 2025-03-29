/**
 * PIMBackend.h
 * Translates LLVM IR to PIM ISA instructions
 */

#ifndef PIM_BACKEND_H
#define PIM_BACKEND_H

#include <memory>
#include <vector>
#include <llvm/IR/Module.h>
#include "PIMInstruction.h"
#include "../include/PIMInstructionSet.h"

class PIMBackend {
public:
    PIMBackend();
    ~PIMBackend();

    /**
     * Generate PIM instructions from LLVM IR
     * 
     * @param module LLVM module to translate
     * @return Vector of PIM instructions
     */
    std::vector<PIMInstruction> generatePIMInstructions(std::unique_ptr<llvm::Module>& module);

private:
    /**
     * Process matrix multiplication patterns in LLVM IR
     * 
     * @param function LLVM function to process
     * @param instructions Vector to add generated instructions to
     */
    void processMatrixMultiplyFunction(llvm::Function* function, std::vector<PIMInstruction>& instructions);
    
    /**
     * Generate instructions for loading matrices into PIM memory
     * 
     * @param instructions Vector to add generated instructions to
     * @param rows Number of rows
     * @param cols Number of columns
     * @param common Common dimension
     */
    void generateMatrixLoadInstructions(std::vector<PIMInstruction>& instructions, 
                                        unsigned rows, unsigned cols, unsigned common);
                                        
    /**
     * Generate matrix multiplication instructions for PIM architecture
     * 
     * @param instructions Vector to add generated instructions to
     * @param rows Number of rows
     * @param cols Number of columns
     * @param common Common dimension
     */
    void generateMatrixMultiplyInstructions(std::vector<PIMInstruction>& instructions,
                                            unsigned rows, unsigned cols, unsigned common);
                                            
    /**
     * Generate store result instructions
     * 
     * @param instructions Vector to add generated instructions to
     * @param rows Number of rows
     * @param cols Number of columns
     */
    void generateStoreResultInstructions(std::vector<PIMInstruction>& instructions,
                                         unsigned rows, unsigned cols);
};

#endif // PIM_BACKEND_H
