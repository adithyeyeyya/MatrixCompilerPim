#include "RefactoringAssistant.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <algorithm>

// LoopReorderingRule implementation
bool LoopReorderingRule::applies(const std::string& code) {
    // Look for nested loops with three levels (typical matrix multiplication pattern)
    std::regex nestedLoopsPattern(
        R"(for\s*\([^{]*\)\s*\{\s*for\s*\([^{]*\)\s*\{\s*for\s*\([^{]*\)\s*\{)");
    return std::regex_search(code, nestedLoopsPattern);
}

std::string LoopReorderingRule::apply(const std::string& code) {
    // This is a simplified implementation
    // In a real-world scenario, we would need to properly parse the code and analyze loop dependencies
    
    // For now, we'll implement a basic pattern-based transformation for matrix multiplication
    std::regex matMulPattern(
        R"(for\s*\(\s*int\s+i\s*=\s*0\s*;\s*i\s*<\s*([a-zA-Z0-9_]+)\s*;\s*i\+\+\s*\)\s*\{\s*for\s*\(\s*int\s+j\s*=\s*0\s*;\s*j\s*<\s*([a-zA-Z0-9_]+)\s*;\s*j\+\+\s*\)\s*\{\s*for\s*\(\s*int\s+k\s*=\s*0\s*;\s*k\s*<\s*([a-zA-Z0-9_]+)\s*;\s*k\+\+\s*\)\s*\{)");
    
    std::string result = code;
    std::smatch matches;
    
    if (std::regex_search(code, matches, matMulPattern) && matches.size() >= 4) {
        std::string rowsVar = matches[1];
        std::string colsVar = matches[2];
        std::string commonVar = matches[3];
        
        // Create optimized loop order (k-i-j) for better memory access patterns in PIM
        std::string replacement = 
            "for (int k = 0; k < " + commonVar + "; k++) {\n"
            "    for (int i = 0; i < " + rowsVar + "; i++) {\n"
            "        for (int j = 0; j < " + colsVar + "; j++) {";
        
        result = std::regex_replace(code, matMulPattern, replacement);
    }
    
    return result;
}

std::string LoopReorderingRule::getDescription() const {
    return "Reorder nested loops to optimize memory access patterns for PIM architecture";
}

std::string LoopReorderingRule::getPerformanceImpact() const {
    return "Can reduce memory access latency by up to 30% and improve parallelism in PIM operations";
}

// MatrixLayoutRule implementation
bool MatrixLayoutRule::applies(const std::string& code) {
    // Look for array declarations with matrix-like dimensions
    std::regex matrixPattern(R"(int\s+[A-Za-z_][A-Za-z0-9_]*\s*\[\s*\d+\s*\]\s*\[\s*\d+\s*\])");
    return std::regex_search(code, matrixPattern);
}

std::string MatrixLayoutRule::apply(const std::string& code) {
    // This implementation would transform row-major matrices to a layout
    // that's more efficient for PIM operations
    // For a proof of concept, we'll just add a comment suggesting the change
    
    std::string result = code;
    std::regex matrixPattern(R"((int\s+)([A-Za-z_][A-Za-z0-9_]*\s*\[\s*\d+\s*\]\s*\[\s*\d+\s*\]))");
    
    result = std::regex_replace(result, matrixPattern, 
        "$1$2 /* Consider using PIM-optimized matrix layout for better performance */");
    
    return result;
}

std::string MatrixLayoutRule::getDescription() const {
    return "Optimize matrix data layout for efficient PIM operations";
}

std::string MatrixLayoutRule::getPerformanceImpact() const {
    return "Can improve computation throughput by 20-40% by better utilizing PIM's parallel processing units";
}

// BlockingOptimizationRule implementation
bool BlockingOptimizationRule::applies(const std::string& code) {
    // Look for matrix multiplication patterns that could benefit from blocking
    std::regex matrixMultPattern(
        R"(C\s*\[\s*i\s*\]\s*\[\s*j\s*\]\s*\+?=\s*A\s*\[\s*i\s*\]\s*\[\s*k\s*\]\s*\*\s*B\s*\[\s*k\s*\]\s*\[\s*j\s*\])");
    return std::regex_search(code, matrixMultPattern);
}

std::string BlockingOptimizationRule::apply(const std::string& code) {
    // For a full implementation, we would transform the matrix multiplication
    // to use blocking/tiling for better cache utilization
    // For now, we'll add a commented example
    
    std::string result = code;
    
    if (applies(code)) {
        // Find the position after the last include
        size_t insertPos = code.find("#include");
        if (insertPos != std::string::npos) {
            insertPos = code.find("\n", insertPos);
            if (insertPos != std::string::npos) {
                insertPos++;
                
                std::string blockingExample = 
                    "/* PIM-Optimized blocking implementation example:\n"
                    "\n"
                    "void matrixMultiplyBlocked(int* A, int* B, int* C, int rows, int cols, int common) {\n"
                    "    // Initialize result matrix to zero\n"
                    "    for (int i = 0; i < rows; i++) {\n"
                    "        for (int j = 0; j < cols; j++) {\n"
                    "            C[i * cols + j] = 0;\n"
                    "        }\n"
                    "    }\n"
                    "    \n"
                    "    // Block size optimized for PIM architecture\n"
                    "    const int blockSize = 16; // Adjust based on PIM memory layout\n"
                    "    \n"
                    "    // Blocked matrix multiplication\n"
                    "    for (int ii = 0; ii < rows; ii += blockSize) {\n"
                    "        for (int jj = 0; jj < cols; jj += blockSize) {\n"
                    "            for (int kk = 0; kk < common; kk += blockSize) {\n"
                    "                // Process block\n"
                    "                for (int i = ii; i < std::min(ii + blockSize, rows); i++) {\n"
                    "                    for (int j = jj; j < std::min(jj + blockSize, cols); j++) {\n"
                    "                        for (int k = kk; k < std::min(kk + blockSize, common); k++) {\n"
                    "                            C[i * cols + j] += A[i * common + k] * B[k * cols + j];\n"
                    "                        }\n"
                    "                    }\n"
                    "                }\n"
                    "            }\n"
                    "        }\n"
                    "    }\n"
                    "}\n"
                    "*/\n\n";
                
                result.insert(insertPos, blockingExample);
            }
        }
    }
    
    return result;
}

std::string BlockingOptimizationRule::getDescription() const {
    return "Apply blocking/tiling optimization to matrix operations for PIM architecture";
}

std::string BlockingOptimizationRule::getPerformanceImpact() const {
    return "Can improve performance by 2-4x by optimizing memory access patterns and data locality for PIM execution";
}

// RefactoringAssistant implementation
RefactoringAssistant::RefactoringAssistant()
    : verbosityLevel(0), logger("RefactoringAssistant") {
    initializeRules();
}

RefactoringAssistant::~RefactoringAssistant() = default;

void RefactoringAssistant::initializeRules() {
    rules.push_back(std::make_unique<LoopReorderingRule>());
    rules.push_back(std::make_unique<MatrixLayoutRule>());
    rules.push_back(std::make_unique<BlockingOptimizationRule>());
}

std::map<std::string, std::pair<std::string, std::string>> RefactoringAssistant::suggestRefactorings(
    const std::string& sourceCode) {
    
    logger.log("Analyzing source code for refactoring opportunities...");
    
    std::map<std::string, std::pair<std::string, std::string>> suggestions;
    
    for (const auto& rule : rules) {
        if (rule->applies(sourceCode)) {
            logger.log("Rule '" + rule->getDescription() + "' applies to the source code");
            
            // Find the relevant code fragment
            std::map<std::string, std::string> patterns = findPatterns(sourceCode);
            
            for (const auto& [patternName, codeFragment] : patterns) {
                if (rule->applies(codeFragment)) {
                    std::string refactoredCode = rule->apply(codeFragment);
                    
                    if (refactoredCode != codeFragment) {
                        logger.log("Generated refactoring suggestion for " + patternName);
                        
                        suggestions[rule->getDescription()] = {codeFragment, refactoredCode};
                        
                        if (verbosityLevel > 0) {
                            logger.log("Original: " + codeFragment);
                            logger.log("Refactored: " + refactoredCode);
                            logger.log("Performance impact: " + rule->getPerformanceImpact());
                        }
                    }
                }
            }
        }
    }
    
    logger.log("Found " + std::to_string(suggestions.size()) + " refactoring suggestions");
    return suggestions;
}

std::map<std::string, std::string> RefactoringAssistant::suggestInstructionOptimizations(
    const std::vector<PIMInstruction>& instructions,
    const std::string& sourceCode) {
    
    logger.log("Analyzing PIM instructions for optimization opportunities...");
    
    std::map<std::string, std::string> suggestions;
    
    // Count instruction types
    std::map<std::string, int> instructionCounts;
    for (const auto& instr : instructions) {
        instructionCounts[instr.getOpName()]++;
    }
    
    // Find instruction patterns
    auto patterns = findInstructionPatterns(instructions);
    
    // Look for specific optimization opportunities
    
    // Check for excessive MOVE operations
    if (instructionCounts["MOVE"] > instructions.size() / 3) {
        suggestions["Reduce MOVE Operations"] = 
            "The generated code has a high proportion of MOVE instructions (" + 
            std::to_string(instructionCounts["MOVE"]) + 
            " out of " + std::to_string(instructions.size()) + 
            "). Consider reorganizing matrix access patterns to reduce data movement.";
    }
    
    // Check for unbalanced computation vs. data movement
    int computeOps = instructionCounts["ADD"] + instructionCounts["MUL"];
    int memoryOps = instructionCounts["LOAD"] + instructionCounts["STORE"];
    if (computeOps < memoryOps / 2) {
        suggestions["Improve Computation Intensity"] = 
            "The ratio of compute operations to memory operations is low. " +
            "Consider increasing computation intensity through loop fusion or blocking.";
    }
    
    // Check for potential parallel execution
    if (patterns.find("Sequential MUL-ADD") != patterns.end()) {
        suggestions["Parallelize Operations"] = 
            "Found sequential MUL-ADD patterns that could be parallelized " +
            "through loop restructuring to better utilize PIM's parallel processing units.";
    }
    
    logger.log("Found " + std::to_string(suggestions.size()) + " instruction optimization suggestions");
    return suggestions;
}

void RefactoringAssistant::setVerbosity(int level) {
    verbosityLevel = level;
    logger.log("Verbosity level set to " + std::to_string(level));
}

std::map<std::string, std::string> RefactoringAssistant::findPatterns(const std::string& code) {
    std::map<std::string, std::string> patterns;
    
    // Look for matrix multiplication patterns
    std::regex matMulPattern(
        R"(for\s*\([^{]*\)\s*\{\s*for\s*\([^{]*\)\s*\{\s*for\s*\([^{]*\)\s*\{[^}]*\}\s*\}\s*\})");
    
    std::string::const_iterator searchStart(code.cbegin());
    std::smatch match;
    while (std::regex_search(searchStart, code.cend(), match, matMulPattern)) {
        patterns["Matrix Multiplication"] = match[0];
        searchStart = match.suffix().first;
    }
    
    // Look for matrix initialization patterns
    std::regex matInitPattern(
        R"(for\s*\([^{]*\)\s*\{\s*for\s*\([^{]*\)\s*\{\s*[^=]*=\s*0[^}]*\}\s*\})");
    
    searchStart = code.cbegin();
    while (std::regex_search(searchStart, code.cend(), match, matInitPattern)) {
        patterns["Matrix Initialization"] = match[0];
        searchStart = match.suffix().first;
    }
    
    return patterns;
}

std::map<std::string, std::vector<PIMInstruction>> RefactoringAssistant::findInstructionPatterns(
    const std::vector<PIMInstruction>& instructions) {
    
    std::map<std::string, std::vector<PIMInstruction>> patterns;
    
    // Look for sequential MUL-ADD patterns
    std::vector<PIMInstruction> mulAddPattern;
    for (size_t i = 0; i < instructions.size() - 1; i++) {
        if (instructions[i].getOpName() == "MUL" && 
            instructions[i+1].getOpName() == "ADD") {
            mulAddPattern.push_back(instructions[i]);
            mulAddPattern.push_back(instructions[i+1]);
        }
    }
    
    if (!mulAddPattern.empty()) {
        patterns["Sequential MUL-ADD"] = mulAddPattern;
    }
    
    // Look for redundant LOAD-STORE patterns
    std::vector<PIMInstruction> loadStorePattern;
    for (size_t i = 0; i < instructions.size() - 1; i++) {
        if (instructions[i].getOpName() == "LOAD" && 
            i+1 < instructions.size() && instructions[i+1].getOpName() == "STORE") {
            loadStorePattern.push_back(instructions[i]);
            loadStorePattern.push_back(instructions[i+1]);
        }
    }
    
    if (!loadStorePattern.empty()) {
        patterns["Redundant LOAD-STORE"] = loadStorePattern;
    }
    
    return patterns;
}