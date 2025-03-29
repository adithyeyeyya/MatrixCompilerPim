/**
 * Main entry point for the PIM Compiler
 * This file implements the command-line interface for the compiler
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "compiler/Parser.h"
#include "compiler/IRGenerator.h"
#include "compiler/PIMBackend.h"
#include "compiler/MemoryMapper.h"
#include "optimizer/RefactoringAssistant.h"
#include "utils/Logger.h"
#include "../include/CompilerConfig.h"

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] input_file\n"
              << "Options:\n"
              << "  -o <file>        Write output to <file>\n"
              << "  -v, --verbose    Enable verbose output\n"
              << "  -h, --help       Display this help message\n"
              << "  --dump-ir        Dump LLVM IR to stderr\n"
              << "  --refactor       Enable AI-powered code refactoring suggestions\n"
              << "  --refactor-only  Only suggest refactoring without compiling\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string inputFile;
    std::string outputFile = "a.out";
    bool verbose = false;
    bool dumpIR = false;
    bool enableRefactoring = false;
    bool refactorOnly = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "--dump-ir") {
            dumpIR = true;
        } else if (arg == "--refactor") {
            enableRefactoring = true;
        } else if (arg == "--refactor-only") {
            enableRefactoring = true;
            refactorOnly = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    // Set up logging
    Logger::getInstance().setVerbose(verbose);
    Logger::getInstance().log("PIM Compiler started");
    Logger::getInstance().log("Input file: " + inputFile);
    Logger::getInstance().log("Output file: " + outputFile);

    try {
        // Read input file
        std::ifstream inFile(inputFile);
        if (!inFile) {
            std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
            return 1;
        }

        std::string source((std::istreambuf_iterator<char>(inFile)),
                           std::istreambuf_iterator<char>());
        inFile.close();
        
        // Process refactoring if enabled
        if (enableRefactoring) {
            Logger::getInstance().log("Running AI-powered code refactoring analysis...");
            std::cout << "\n=== PIM Architecture Code Refactoring Assistant ===\n";
            
            RefactoringAssistant assistant;
            if (verbose) {
                assistant.setVerbosity(1);
            }
            
            auto suggestions = assistant.suggestRefactorings(source);
            
            if (suggestions.empty()) {
                std::cout << "No refactoring suggestions found for the provided code.\n";
                std::cout << "The code appears to be already well-optimized for PIM architecture.\n";
            } else {
                std::cout << "Found " << suggestions.size() << " potential optimizations:\n\n";
                
                int i = 1;
                for (const auto& [description, codePair] : suggestions) {
                    const auto& [originalCode, refactoredCode] = codePair;
                    
                    std::cout << "Suggestion " << i << ": " << description << "\n";
                    std::cout << "-------------------------------------\n";
                    std::cout << "Original code:\n";
                    std::cout << originalCode << "\n\n";
                    std::cout << "Suggested refactoring:\n";
                    std::cout << refactoredCode << "\n\n";
                    
                    i++;
                }
                
                // Create a refactored version of the source
                std::string refactoredSource = source;
                for (const auto& [description, codePair] : suggestions) {
                    const auto& [originalCode, refactoredCode] = codePair;
                    size_t pos = refactoredSource.find(originalCode);
                    if (pos != std::string::npos) {
                        refactoredSource.replace(pos, originalCode.length(), refactoredCode);
                    }
                }
                
                // Write refactored version to file
                std::string refactoredFilename = inputFile.substr(0, inputFile.find_last_of('.')) + "_refactored.cpp";
                std::ofstream refactoredFile(refactoredFilename);
                if (refactoredFile) {
                    refactoredFile << refactoredSource;
                    refactoredFile.close();
                    std::cout << "Refactored code written to: " << refactoredFilename << "\n";
                }
            }
            
            if (refactorOnly) {
                return 0;
            }
            
            std::cout << "\n=== Continuing with compilation ===\n\n";
        }

        // Create compiler pipeline components
        Parser parser;
        IRGenerator irGenerator;
        MemoryMapper memoryMapper;
        PIMBackend backend;

        // Execute compilation pipeline
        Logger::getInstance().log("Parsing input file...");
        std::unique_ptr<llvm::Module> module;
        
#ifdef HAVE_CLANG
        auto ast = parser.parse(source);
        Logger::getInstance().log("Generating LLVM IR...");
        module = irGenerator.generateIR(ast);
#else
        try {
            void* dummyAst = parser.parse(source);
            Logger::getInstance().log("Generating LLVM IR...");
            module = irGenerator.generateIR(dummyAst);
        } catch (const std::runtime_error& e) {
            Logger::getInstance().log("Using fallback path: Clang not available");
            // Pass nullptr as a void* directly to the IR generator
            module = irGenerator.generateIR(nullptr);
        }
#endif
        
        if (dumpIR) {
            irGenerator.dumpIR(module);
        }
        
        Logger::getInstance().log("Applying memory mapping for PIM architecture...");
        auto mappedModule = memoryMapper.applyMemoryMapping(module);
        
        Logger::getInstance().log("Generating PIM instructions...");
        auto instructions = backend.generatePIMInstructions(mappedModule);
        
        // Add instruction-level optimization suggestions if refactoring is enabled
        if (enableRefactoring) {
            Logger::getInstance().log("Analyzing generated PIM instructions...");
            std::cout << "\n=== PIM Instruction Optimization Analysis ===\n";
            
            RefactoringAssistant assistant;
            auto instructionSuggestions = assistant.suggestInstructionOptimizations(instructions, source);
            
            if (instructionSuggestions.empty()) {
                std::cout << "No instruction-level optimization suggestions found.\n";
                std::cout << "The generated PIM code appears to be already well-optimized.\n";
            } else {
                std::cout << "Found " << instructionSuggestions.size() << " potential instruction-level optimizations:\n\n";
                
                int i = 1;
                for (const auto& [name, suggestion] : instructionSuggestions) {
                    std::cout << "Suggestion " << i << ": " << name << "\n";
                    std::cout << "-------------------------------------\n";
                    std::cout << suggestion << "\n\n";
                    i++;
                }
            }
        }
        
        // Write output to file
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error: Could not open output file: " << outputFile << std::endl;
            return 1;
        }
        
        for (const auto& instruction : instructions) {
            outFile << instruction.toString() << "\n";
        }
        outFile.close();
        
        Logger::getInstance().log("Compilation completed successfully");
        std::cout << "Compiled " << inputFile << " to " << outputFile << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
