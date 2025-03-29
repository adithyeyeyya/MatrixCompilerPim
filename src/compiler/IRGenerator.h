/**
 * IRGenerator.h
 * Generates LLVM IR from the AST
 */

#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <memory>
#include <string>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

// Include Clang headers conditionally
#ifdef HAVE_CLANG
#include <clang/AST/ASTContext.h>
#endif

// Forward declarations for Clang classes if not using Clang
#ifndef HAVE_CLANG
namespace clang {
    class ASTContext;
    class FunctionDecl;
    class Stmt;
}
#endif

class IRGenerator {
public:
    IRGenerator();
    ~IRGenerator();

#ifdef HAVE_CLANG
    /**
     * Generate LLVM IR from AST
     * 
     * @param astContext The AST context from which to generate IR
     * @return Generated LLVM module
     * @throws std::runtime_error if IR generation fails
     */
    std::unique_ptr<llvm::Module> generateIR(std::unique_ptr<clang::ASTContext>& astContext);
#else
    /**
     * Generate LLVM IR from void* (dummy AST)
     * 
     * @param astContext Dummy AST pointer (unused)
     * @return Generated LLVM module
     */
    std::unique_ptr<llvm::Module> generateIR(void* astContext);
#endif
    
    /**
     * Dump LLVM IR to stdout for debugging
     */
    void dumpIR(std::unique_ptr<llvm::Module>& module);

private:
    std::unique_ptr<llvm::LLVMContext> llvmContext;
    
    /**
     * Create a hardcoded matrix multiplication function
     * Used when Clang is not available
     */
    void createHardcodedMatrixMultiplyFunction(llvm::Module* module, llvm::IRBuilder<>& builder);
    
#ifdef HAVE_CLANG
    /**
     * Process a matrix multiplication function
     * Identifies matrix multiplication patterns and emits appropriate IR
     */
    void processMatrixMultiplication(clang::FunctionDecl* func, llvm::Module* module, llvm::IRBuilder<>& builder);
    
    /**
     * Detect matrix multiply pattern in the given statement
     */
    bool isMatrixMultiplyPattern(clang::Stmt* stmt);
#endif
};

#endif // IR_GENERATOR_H
