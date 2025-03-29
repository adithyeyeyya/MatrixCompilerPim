/**
 * IRGenerator.cpp
 * Implements the generation of LLVM IR from AST
 */

#include "IRGenerator.h"
#include "../utils/Logger.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#ifdef HAVE_CLANG
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclVisitor.h>
#include <clang/AST/StmtVisitor.h>

class MatrixMultVisitor : public clang::RecursiveASTVisitor<MatrixMultVisitor> {
private:
    IRGenerator* generator;
    llvm::Module* module;
    llvm::IRBuilder<>* builder;

public:
    MatrixMultVisitor(IRGenerator* generator, llvm::Module* module, llvm::IRBuilder<>* builder)
        : generator(generator), module(module), builder(builder) {}

    bool VisitFunctionDecl(clang::FunctionDecl* decl) {
        if (decl->isThisDeclarationADefinition()) {
            Logger::getInstance().log("Processing function: " + decl->getNameAsString());
            generator->processMatrixMultiplication(decl, module, *builder);
        }
        return true;
    }
};
#endif

IRGenerator::IRGenerator() {
    llvmContext = std::make_unique<llvm::LLVMContext>();
}

IRGenerator::~IRGenerator() = default;

#ifdef HAVE_CLANG
std::unique_ptr<llvm::Module> IRGenerator::generateIR(std::unique_ptr<clang::ASTContext>& astContext) {
    if (!astContext) {
        throw std::runtime_error("AST Context is null");
    }
    
    Logger::getInstance().log("Starting LLVM IR generation");
    
    // Create new LLVM module
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("MatrixMultiplicationModule", *llvmContext);
    llvm::IRBuilder<> builder(*llvmContext);
    
    // Visit the AST to identify matrix multiplication patterns
    MatrixMultVisitor visitor(this, module.get(), &builder);
    visitor.TraverseDecl(astContext->getTranslationUnitDecl());
    
    Logger::getInstance().log("LLVM IR generation completed");
    
    return module;
}
#else
std::unique_ptr<llvm::Module> IRGenerator::generateIR(void* /*astContext*/) {
    Logger::getInstance().log("Starting LLVM IR generation (no Clang available)");
    
    // Create new LLVM module
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("MatrixMultiplicationModule", *llvmContext);
    llvm::IRBuilder<> builder(*llvmContext);
    
    // When Clang is not available, create a hardcoded matrix multiplication function
    createHardcodedMatrixMultiplyFunction(module.get(), builder);
    
    Logger::getInstance().log("LLVM IR generation completed (fallback path)");
    
    return module;
}
#endif

void IRGenerator::dumpIR(std::unique_ptr<llvm::Module>& module) {
    Logger::getInstance().log("Dumping LLVM IR");
    std::string irStr;
    llvm::raw_string_ostream irStream(irStr);
    module->print(irStream, nullptr);
    std::cerr << irStr;
}

#ifdef HAVE_CLANG
void IRGenerator::processMatrixMultiplication(clang::FunctionDecl* func, llvm::Module* module, llvm::IRBuilder<>& builder) {
    // Skip if function body is null
    if (!func->hasBody()) {
        return;
    }
    
    clang::Stmt* body = func->getBody();
    
    // Create LLVM function
    std::vector<llvm::Type*> paramTypes;
    for (auto param : func->parameters()) {
        llvm::Type* paramType = llvm::Type::getInt32Ty(*llvmContext);
        if (param->getType()->isPointerType()) {
            paramType = llvm::Type::getInt32PtrTy(*llvmContext);
        }
        paramTypes.push_back(paramType);
    }
    
    llvm::Type* returnType = llvm::Type::getVoidTy(*llvmContext);
    if (!func->getReturnType()->isVoidType()) {
        returnType = llvm::Type::getInt32Ty(*llvmContext);
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, func->getNameAsString(), module);
    
    // Create entry basic block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*llvmContext, "entry", llvmFunc);
    builder.SetInsertPoint(entryBB);
    
    // Process function body for matrix multiplication patterns
    for (auto it = body->child_begin(); it != body->child_end(); ++it) {
        if (isMatrixMultiplyPattern(*it)) {
            Logger::getInstance().log("Found matrix multiplication pattern");
            // Here we would extract the matrices and their dimensions
            // and generate the appropriate LLVM IR for matrix multiplication
            
            // For now, we'll create a simple matrix multiplication IR structure
            // This is a simplified placeholder for the actual implementation
            
            // Create sample parameters for 2x2 matrices
            llvm::Value* matrixA = llvm::ConstantPointerNull::get(llvm::Type::getInt32PtrTy(*llvmContext));
            llvm::Value* matrixB = llvm::ConstantPointerNull::get(llvm::Type::getInt32PtrTy(*llvmContext));
            llvm::Value* matrixC = llvm::ConstantPointerNull::get(llvm::Type::getInt32PtrTy(*llvmContext));
            llvm::Value* rows = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 2);
            llvm::Value* cols = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 2);
            llvm::Value* common = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 2);
            
            // Create nested loops for matrix multiplication
            // Outer loop (i)
            llvm::Value* i = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "i");
            builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), i);
            
            llvm::BasicBlock* outerLoopCond = llvm::BasicBlock::Create(*llvmContext, "outer_loop_cond", llvmFunc);
            llvm::BasicBlock* outerLoopBody = llvm::BasicBlock::Create(*llvmContext, "outer_loop_body", llvmFunc);
            llvm::BasicBlock* outerLoopInc = llvm::BasicBlock::Create(*llvmContext, "outer_loop_inc", llvmFunc);
            llvm::BasicBlock* outerLoopEnd = llvm::BasicBlock::Create(*llvmContext, "outer_loop_end", llvmFunc);
            
            builder.CreateBr(outerLoopCond);
            
            // Outer loop condition
            builder.SetInsertPoint(outerLoopCond);
            llvm::Value* iVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), i, "i_val");
            llvm::Value* iCond = builder.CreateICmpSLT(iVal, rows, "i_cond");
            builder.CreateCondBr(iCond, outerLoopBody, outerLoopEnd);
            
            // Outer loop body
            builder.SetInsertPoint(outerLoopBody);
            
            // Middle loop (j)
            llvm::Value* j = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "j");
            builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), j);
            
            llvm::BasicBlock* middleLoopCond = llvm::BasicBlock::Create(*llvmContext, "middle_loop_cond", llvmFunc);
            llvm::BasicBlock* middleLoopBody = llvm::BasicBlock::Create(*llvmContext, "middle_loop_body", llvmFunc);
            llvm::BasicBlock* middleLoopInc = llvm::BasicBlock::Create(*llvmContext, "middle_loop_inc", llvmFunc);
            llvm::BasicBlock* middleLoopEnd = llvm::BasicBlock::Create(*llvmContext, "middle_loop_end", llvmFunc);
            
            builder.CreateBr(middleLoopCond);
            
            // Middle loop condition
            builder.SetInsertPoint(middleLoopCond);
            llvm::Value* jVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), j, "j_val");
            llvm::Value* jCond = builder.CreateICmpSLT(jVal, cols, "j_cond");
            builder.CreateCondBr(jCond, middleLoopBody, middleLoopEnd);
            
            // Middle loop body
            builder.SetInsertPoint(middleLoopBody);
            
            // Initialize C[i][j] = 0
            llvm::Value* cij_idx = builder.CreateAdd(
                builder.CreateMul(iVal, cols, "i_cols"),
                jVal, "cij_idx"
            );
            llvm::Value* cij_ptr = builder.CreateGEP(
                llvm::Type::getInt32Ty(*llvmContext),
                matrixC, cij_idx, "cij_ptr"
            );
            builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), cij_ptr);
            
            // Inner loop (k)
            llvm::Value* k = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "k");
            builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), k);
            
            llvm::BasicBlock* innerLoopCond = llvm::BasicBlock::Create(*llvmContext, "inner_loop_cond", llvmFunc);
            llvm::BasicBlock* innerLoopBody = llvm::BasicBlock::Create(*llvmContext, "inner_loop_body", llvmFunc);
            llvm::BasicBlock* innerLoopInc = llvm::BasicBlock::Create(*llvmContext, "inner_loop_inc", llvmFunc);
            llvm::BasicBlock* innerLoopEnd = llvm::BasicBlock::Create(*llvmContext, "inner_loop_end", llvmFunc);
            
            builder.CreateBr(innerLoopCond);
            
            // Inner loop condition
            builder.SetInsertPoint(innerLoopCond);
            llvm::Value* kVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), k, "k_val");
            llvm::Value* kCond = builder.CreateICmpSLT(kVal, common, "k_cond");
            builder.CreateCondBr(kCond, innerLoopBody, innerLoopEnd);
            
            // Inner loop body
            builder.SetInsertPoint(innerLoopBody);
            
            // Calculate A[i][k]
            llvm::Value* aik_idx = builder.CreateAdd(
                builder.CreateMul(iVal, common, "i_common"),
                kVal, "aik_idx"
            );
            llvm::Value* aik_ptr = builder.CreateGEP(
                llvm::Type::getInt32Ty(*llvmContext),
                matrixA, aik_idx, "aik_ptr"
            );
            llvm::Value* aik_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), aik_ptr, "aik_val");
            
            // Calculate B[k][j]
            llvm::Value* bkj_idx = builder.CreateAdd(
                builder.CreateMul(kVal, cols, "k_cols"),
                jVal, "bkj_idx"
            );
            llvm::Value* bkj_ptr = builder.CreateGEP(
                llvm::Type::getInt32Ty(*llvmContext),
                matrixB, bkj_idx, "bkj_ptr"
            );
            llvm::Value* bkj_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), bkj_ptr, "bkj_val");
            
            // Multiply A[i][k] * B[k][j]
            llvm::Value* mul_result = builder.CreateMul(aik_val, bkj_val, "mul_result");
            
            // Add to C[i][j]
            llvm::Value* cij_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), cij_ptr, "cij_val");
            llvm::Value* add_result = builder.CreateAdd(cij_val, mul_result, "add_result");
            builder.CreateStore(add_result, cij_ptr);
            
            // Inner loop increment
            builder.CreateBr(innerLoopInc);
            builder.SetInsertPoint(innerLoopInc);
            llvm::Value* kNext = builder.CreateAdd(kVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "k_next");
            builder.CreateStore(kNext, k);
            builder.CreateBr(innerLoopCond);
            
            // Inner loop end
            builder.SetInsertPoint(innerLoopEnd);
            
            // Middle loop increment
            builder.CreateBr(middleLoopInc);
            builder.SetInsertPoint(middleLoopInc);
            llvm::Value* jNext = builder.CreateAdd(jVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "j_next");
            builder.CreateStore(jNext, j);
            builder.CreateBr(middleLoopCond);
            
            // Middle loop end
            builder.SetInsertPoint(middleLoopEnd);
            
            // Outer loop increment
            builder.CreateBr(outerLoopInc);
            builder.SetInsertPoint(outerLoopInc);
            llvm::Value* iNext = builder.CreateAdd(iVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "i_next");
            builder.CreateStore(iNext, i);
            builder.CreateBr(outerLoopCond);
            
            // Outer loop end
            builder.SetInsertPoint(outerLoopEnd);
        }
    }
    
    // Complete the function
    if (returnType->isVoidTy()) {
        builder.CreateRetVoid();
    } else {
        builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0));
    }
}

bool IRGenerator::isMatrixMultiplyPattern(clang::Stmt* stmt) {
    // In a real implementation, this would analyze the statement to detect matrix multiplication
    // We look for triple-nested loops with array accesses of the form C[i][j] += A[i][k] * B[k][j]
    
    // For this implementation, we'll make a simplified check
    // We're just checking for the presence of a ForStmt - this is a placeholder
    return llvm::isa<clang::ForStmt>(stmt);
}
#endif

// Helper function for when Clang is not available
void IRGenerator::createHardcodedMatrixMultiplyFunction(llvm::Module* module, llvm::IRBuilder<>& builder) {
    Logger::getInstance().log("Creating hardcoded matrix multiplication function (Clang not available)");
    
    // Create function prototype for matrix multiplication
    std::vector<llvm::Type*> paramTypes = {
        llvm::Type::getInt32PtrTy(*llvmContext),  // Matrix A
        llvm::Type::getInt32PtrTy(*llvmContext),  // Matrix B
        llvm::Type::getInt32PtrTy(*llvmContext),  // Matrix C
        llvm::Type::getInt32Ty(*llvmContext),     // Rows
        llvm::Type::getInt32Ty(*llvmContext),     // Cols
        llvm::Type::getInt32Ty(*llvmContext)      // Common dimension
    };
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*llvmContext), paramTypes, false);
    
    llvm::Function* matrixMultFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "matrixMultiply", module);
    
    // Set parameter names
    auto argIt = matrixMultFunc->arg_begin();
    llvm::Value* matrixA = &*(argIt++); matrixA->setName("A");
    llvm::Value* matrixB = &*(argIt++); matrixB->setName("B");
    llvm::Value* matrixC = &*(argIt++); matrixC->setName("C");
    llvm::Value* rows = &*(argIt++); rows->setName("rows");
    llvm::Value* cols = &*(argIt++); cols->setName("cols");
    llvm::Value* common = &*(argIt++); common->setName("common");
    
    // Create the entry basic block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*llvmContext, "entry", matrixMultFunc);
    builder.SetInsertPoint(entryBB);
    
    // Create nested loops for matrix multiplication - similar to the Clang-dependent version
    // Outer loop (i)
    llvm::Value* i = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "i");
    builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), i);
    
    llvm::BasicBlock* outerLoopCond = llvm::BasicBlock::Create(*llvmContext, "outer_loop_cond", matrixMultFunc);
    llvm::BasicBlock* outerLoopBody = llvm::BasicBlock::Create(*llvmContext, "outer_loop_body", matrixMultFunc);
    llvm::BasicBlock* outerLoopInc = llvm::BasicBlock::Create(*llvmContext, "outer_loop_inc", matrixMultFunc);
    llvm::BasicBlock* outerLoopEnd = llvm::BasicBlock::Create(*llvmContext, "outer_loop_end", matrixMultFunc);
    
    builder.CreateBr(outerLoopCond);
    
    // Outer loop condition
    builder.SetInsertPoint(outerLoopCond);
    llvm::Value* iVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), i, "i_val");
    llvm::Value* iCond = builder.CreateICmpSLT(iVal, rows, "i_cond");
    builder.CreateCondBr(iCond, outerLoopBody, outerLoopEnd);
    
    // Outer loop body
    builder.SetInsertPoint(outerLoopBody);
    
    // Middle loop (j)
    llvm::Value* j = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "j");
    builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), j);
    
    llvm::BasicBlock* middleLoopCond = llvm::BasicBlock::Create(*llvmContext, "middle_loop_cond", matrixMultFunc);
    llvm::BasicBlock* middleLoopBody = llvm::BasicBlock::Create(*llvmContext, "middle_loop_body", matrixMultFunc);
    llvm::BasicBlock* middleLoopInc = llvm::BasicBlock::Create(*llvmContext, "middle_loop_inc", matrixMultFunc);
    llvm::BasicBlock* middleLoopEnd = llvm::BasicBlock::Create(*llvmContext, "middle_loop_end", matrixMultFunc);
    
    builder.CreateBr(middleLoopCond);
    
    // Middle loop condition
    builder.SetInsertPoint(middleLoopCond);
    llvm::Value* jVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), j, "j_val");
    llvm::Value* jCond = builder.CreateICmpSLT(jVal, cols, "j_cond");
    builder.CreateCondBr(jCond, middleLoopBody, middleLoopEnd);
    
    // Middle loop body
    builder.SetInsertPoint(middleLoopBody);
    
    // Initialize C[i][j] = 0
    llvm::Value* cij_idx = builder.CreateAdd(
        builder.CreateMul(iVal, cols, "i_cols"),
        jVal, "cij_idx"
    );
    llvm::Value* cij_ptr = builder.CreateGEP(
        llvm::Type::getInt32Ty(*llvmContext),
        matrixC, cij_idx, "cij_ptr"
    );
    builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), cij_ptr);
    
    // Inner loop (k)
    llvm::Value* k = builder.CreateAlloca(llvm::Type::getInt32Ty(*llvmContext), nullptr, "k");
    builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 0), k);
    
    llvm::BasicBlock* innerLoopCond = llvm::BasicBlock::Create(*llvmContext, "inner_loop_cond", matrixMultFunc);
    llvm::BasicBlock* innerLoopBody = llvm::BasicBlock::Create(*llvmContext, "inner_loop_body", matrixMultFunc);
    llvm::BasicBlock* innerLoopInc = llvm::BasicBlock::Create(*llvmContext, "inner_loop_inc", matrixMultFunc);
    llvm::BasicBlock* innerLoopEnd = llvm::BasicBlock::Create(*llvmContext, "inner_loop_end", matrixMultFunc);
    
    builder.CreateBr(innerLoopCond);
    
    // Inner loop condition
    builder.SetInsertPoint(innerLoopCond);
    llvm::Value* kVal = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), k, "k_val");
    llvm::Value* kCond = builder.CreateICmpSLT(kVal, common, "k_cond");
    builder.CreateCondBr(kCond, innerLoopBody, innerLoopEnd);
    
    // Inner loop body
    builder.SetInsertPoint(innerLoopBody);
    
    // Calculate A[i][k]
    llvm::Value* aik_idx = builder.CreateAdd(
        builder.CreateMul(iVal, common, "i_common"),
        kVal, "aik_idx"
    );
    llvm::Value* aik_ptr = builder.CreateGEP(
        llvm::Type::getInt32Ty(*llvmContext),
        matrixA, aik_idx, "aik_ptr"
    );
    llvm::Value* aik_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), aik_ptr, "aik_val");
    
    // Calculate B[k][j]
    llvm::Value* bkj_idx = builder.CreateAdd(
        builder.CreateMul(kVal, cols, "k_cols"),
        jVal, "bkj_idx"
    );
    llvm::Value* bkj_ptr = builder.CreateGEP(
        llvm::Type::getInt32Ty(*llvmContext),
        matrixB, bkj_idx, "bkj_ptr"
    );
    llvm::Value* bkj_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), bkj_ptr, "bkj_val");
    
    // Multiply A[i][k] * B[k][j]
    llvm::Value* mul_result = builder.CreateMul(aik_val, bkj_val, "mul_result");
    
    // Add to C[i][j]
    llvm::Value* cij_val = builder.CreateLoad(llvm::Type::getInt32Ty(*llvmContext), cij_ptr, "cij_val");
    llvm::Value* add_result = builder.CreateAdd(cij_val, mul_result, "add_result");
    builder.CreateStore(add_result, cij_ptr);
    
    // Inner loop increment
    builder.CreateBr(innerLoopInc);
    builder.SetInsertPoint(innerLoopInc);
    llvm::Value* kNext = builder.CreateAdd(kVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "k_next");
    builder.CreateStore(kNext, k);
    builder.CreateBr(innerLoopCond);
    
    // Inner loop end
    builder.SetInsertPoint(innerLoopEnd);
    
    // Middle loop increment
    builder.CreateBr(middleLoopInc);
    builder.SetInsertPoint(middleLoopInc);
    llvm::Value* jNext = builder.CreateAdd(jVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "j_next");
    builder.CreateStore(jNext, j);
    builder.CreateBr(middleLoopCond);
    
    // Middle loop end
    builder.SetInsertPoint(middleLoopEnd);
    
    // Outer loop increment
    builder.CreateBr(outerLoopInc);
    builder.SetInsertPoint(outerLoopInc);
    llvm::Value* iNext = builder.CreateAdd(iVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), 1), "i_next");
    builder.CreateStore(iNext, i);
    builder.CreateBr(outerLoopCond);
    
    // Outer loop end
    builder.SetInsertPoint(outerLoopEnd);
    
    // Complete the function
    builder.CreateRetVoid();
}
