#include "optimizer/RefactoringAssistant.h"
#include "utils/Logger.h"
#include <regex>
#include <iostream>
#include <algorithm>
#include <sstream>

// Implementation of LoopReorderingRule

bool LoopReorderingRule::applies(const std::string& code) {
    // Look for matrix multiplication with i-j-k loop order
    std::regex pattern(
        "for\\s*\\(.*?\\bi\\b.*?\\)\\s*\\{"
        "\\s*for\\s*\\(.*?\\bj\\b.*?\\)\\s*\\{"
        "\\s*for\\s*\\(.*?\\bk\\b.*?\\)\\s*\\{"
    );
    return std::regex_search(code, pattern);
}

std::string LoopReorderingRule::apply(const std::string& code) {
    // Replace i-j-k loop order with i-k-j loop order
    std::regex pattern(
        "(for\\s*\\(.*?\\bi\\b.*?\\)\\s*\\{)"
        "(\\s*for\\s*\\(.*?\\bj\\b.*?\\)\\s*\\{)"
        "(\\s*for\\s*\\(.*?\\bk\\b.*?\\)\\s*\\{)"
    );
    
    std::string result = std::regex_replace(code, pattern, 
        "$1"
        "\n    for (int k = 0; k < common; k++) {"
        "\n        for (int j = 0; j < cols; j++) {"
    );
    
    return result;
}

std::string LoopReorderingRule::getDescription() const {
    return "Reorder loops for better cache locality in matrix multiplication";
}

std::string LoopReorderingRule::getPerformanceImpact() const {
    return "Up to 2-3x speedup due to improved cache utilization and reduced memory access in the inner loop";
}

// Implementation of MatrixLayoutRule

bool MatrixLayoutRule::applies(const std::string& code) {
    // Look for matrix access patterns that could be optimized
    std::regex pattern("A\\s*\\[\\s*i\\s*\\]\\s*\\[\\s*k\\s*\\]|A\\s*\\[\\s*i\\s*\\*\\s*\\w+\\s*\\+\\s*k\\s*\\]");
    return std::regex_search(code, pattern);
}

std::string MatrixLayoutRule::apply(const std::string& code) {
    // Suggest using a transposed layout for matrix B
    std::string result = code;
    
    // Add transpose code before the multiplication
    std::regex mainPattern("void\\s+matrix\\w+\\s*\\([^)]*\\)\\s*\\{");
    if (std::regex_search(code, mainPattern)) {
        std::regex initPattern("(\\s*// Initialize result matrix.*?\n)(\\s*for\\s*\\(.*?\\))");
        if (std::regex_search(code, initPattern)) {
            result = std::regex_replace(result, initPattern,
                "$1"
                "\n    // Transpose matrix B for better memory access patterns"
                "\n    int B_transposed[common][cols];"
                "\n    for (int k = 0; k < common; k++) {"
                "\n        for (int j = 0; j < cols; j++) {"
                "\n            B_transposed[k][j] = B[k][j];"
                "\n        }"
                "\n    }"
                "\n$2"
            );
        }
        
        // Replace access to B in the multiplication loop
        std::regex accessPattern("B\\s*\\[\\s*k\\s*\\]\\s*\\[\\s*j\\s*\\]|B\\s*\\[\\s*k\\s*\\*\\s*\\w+\\s*\\+\\s*j\\s*\\]");
        result = std::regex_replace(result, accessPattern, "B_transposed[k][j]");
    }
    
    return result;
}

std::string MatrixLayoutRule::getDescription() const {
    return "Optimize matrix layout for PIM architecture by using transposed matrices";
}

std::string MatrixLayoutRule::getPerformanceImpact() const {
    return "Potential 20-30% improvement in memory access efficiency, reduced cache misses, and improved parallelism";
}

// Implementation of BlockingOptimizationRule

bool BlockingOptimizationRule::applies(const std::string& code) {
    // Look for large matrix multiplication patterns without blocking
    std::regex pattern("for\\s*\\(.*?\\bi\\b.*?\\)\\s*\\{[^}]*for\\s*\\(.*?\\bj\\b.*?\\)\\s*\\{[^}]*for\\s*\\(.*?\\bk\\b.*?\\)");
    return std::regex_search(code, pattern);
}

std::string BlockingOptimizationRule::apply(const std::string& code) {
    // Apply blocking optimization
    std::string result = code;
    
    // Define block size constant
    std::regex funcPattern("void\\s+matrix\\w+\\s*\\([^)]*\\)\\s*\\{");
    if (std::regex_search(result, funcPattern)) {
        result = std::regex_replace(result, funcPattern, 
            "void matrixMultiplyBlocked(int* A, int* B, int* C, int rows, int cols, int common) {\n"
            "    const int BLOCK_SIZE = 8; // Optimal block size for PIM architecture\n"
        );
    }
    
    // Replace the loops with blocked version
    std::regex loopPattern(
        "for\\s*\\(\\s*int\\s+i\\s*=\\s*0\\s*;\\s*i\\s*<\\s*\\w+\\s*;\\s*i\\+\\+\\s*\\)\\s*\\{"
        "[^}]*for\\s*\\(\\s*int\\s+j\\s*=\\s*0\\s*;\\s*j\\s*<\\s*\\w+\\s*;\\s*j\\+\\+\\s*\\)\\s*\\{"
        "[^}]*for\\s*\\(\\s*int\\s+k\\s*=\\s*0\\s*;\\s*k\\s*<\\s*\\w+\\s*;\\s*k\\+\\+\\s*\\)\\s*\\{"
    );
    
    if (std::regex_search(result, loopPattern)) {
        result = std::regex_replace(result, loopPattern,
            "// Use blocked matrix multiplication algorithm\n"
            "    for (int i = 0; i < rows; i += BLOCK_SIZE) {\n"
            "        for (int j = 0; j < cols; j += BLOCK_SIZE) {\n"
            "            for (int k = 0; k < common; k += BLOCK_SIZE) {\n"
            "                // Process block\n"
            "                for (int ii = i; ii < std::min(i + BLOCK_SIZE, rows); ii++) {\n"
            "                    for (int jj = j; jj < std::min(j + BLOCK_SIZE, cols); jj++) {\n"
            "                        for (int kk = k; kk < std::min(k + BLOCK_SIZE, common); kk++) {"
        );
        
        // Update array access pattern
        std::regex accessPattern("C\\[\\s*i\\s*\\]\\[\\s*j\\s*\\]\\s*\\+=\\s*A\\[\\s*i\\s*\\]\\[\\s*k\\s*\\]\\s*\\*\\s*B\\[\\s*k\\s*\\]\\[\\s*j\\s*\\]");
        result = std::regex_replace(result, accessPattern, "C[ii][jj] += A[ii][kk] * B[kk][jj]");
        
        // Close the additional brackets
        result = std::regex_replace(result, std::regex("(\\s*}\\s*}\\s*})"), "$1\n                }\n            }\n        }");
    }
    
    return result;
}

std::string BlockingOptimizationRule::getDescription() const {
    return "Apply blocking/tiling optimization to improve cache utilization";
}

std::string BlockingOptimizationRule::getPerformanceImpact() const {
    return "Up to 4x speedup for large matrices by maximizing PIM architecture data locality";
}

// Implementation of RefactoringAssistant

RefactoringAssistant::RefactoringAssistant() : verbosityLevel(0) {
    // Initialize the rules
    initializeRules();
}

RefactoringAssistant::~RefactoringAssistant() {
    // Nothing to clean up
}

void RefactoringAssistant::initializeRules() {
    rules.push_back(std::make_unique<LoopReorderingRule>());
    rules.push_back(std::make_unique<MatrixLayoutRule>());
    rules.push_back(std::make_unique<BlockingOptimizationRule>());
}

void RefactoringAssistant::setVerbosity(int level) {
    verbosityLevel = level;
}

std::map<std::string, std::pair<std::string, std::string>> RefactoringAssistant::suggestRefactorings(
    const std::string& sourceCode) {
    
    std::map<std::string, std::pair<std::string, std::string>> suggestions;
    
    // Find patterns in the code
    auto patterns = findPatterns(sourceCode);
    
    // Apply rules to each pattern
    for (const auto& [patternName, pattern] : patterns) {
        for (const auto& rule : rules) {
            if (rule->applies(pattern)) {
                std::string refactored = rule->apply(pattern);
                
                // Only suggest if there's an actual change
                if (refactored != pattern) {
                    std::string description = rule->getDescription() + 
                                             " (" + rule->getPerformanceImpact() + ")";
                    
                    suggestions[description] = std::make_pair(pattern, refactored);
                }
            }
        }
    }
    
    return suggestions;
}

std::map<std::string, std::string> RefactoringAssistant::suggestInstructionOptimizations(
    const std::vector<PIMInstruction>& instructions,
    const std::string& sourceCode) {
    
    std::map<std::string, std::string> suggestions;
    
    // Find patterns in the instructions
    auto patterns = findInstructionPatterns(instructions);
    
    // Analyze instruction patterns
    for (const auto& [patternName, patternInstructions] : patterns) {
        // Example: detect redundant load/store operations
        if (patternName == "redundant_loads") {
            std::stringstream suggestion;
            suggestion << "Identified redundant LOAD operations in the instruction sequence.\n";
            suggestion << "Consider refactoring the source code to avoid reloading the same data.\n";
            suggestion << "Look for opportunities to reuse already loaded values, particularly in nested loops.\n\n";
            
            // Example instruction sequence with issue
            suggestion << "Example problematic instruction sequence:\n";
            for (const auto& instr : patternInstructions) {
                suggestion << "  " << instr.toString() << "\n";
            }
            
            suggestion << "\nPotential impact: Reducing redundant LOADs can improve execution time by 15-20%.";
            
            suggestions["Redundant Memory Operations"] = suggestion.str();
        }
        // Example: detect inefficient use of accumulators
        else if (patternName == "inefficient_accumulation") {
            std::stringstream suggestion;
            suggestion << "Detected inefficient use of accumulator registers in matrix multiplication.\n";
            suggestion << "Consider using register blocking techniques in your code.\n";
            suggestion << "This optimization allows reuse of values in PIM accumulator registers.\n\n";
            
            // Example instruction sequence with issue
            suggestion << "Example instruction sequence with optimization opportunity:\n";
            for (const auto& instr : patternInstructions) {
                suggestion << "  " << instr.toString() << "\n";
            }
            
            suggestion << "\nPotential impact: Efficient accumulator usage can reduce instruction count by 25%.";
            
            suggestions["Inefficient Accumulator Usage"] = suggestion.str();
        }
    }
    
    return suggestions;
}

std::map<std::string, std::string> RefactoringAssistant::findPatterns(const std::string& code) {
    std::map<std::string, std::string> patterns;
    
    // Find matrix multiplication function patterns
    std::regex matrixMultPattern(
        "void\\s+matrix\\w+\\([^)]*\\)\\s*\\{"
        "[^}]*?"  // Non-greedy match of anything
        "for\\s*\\([^{]*\\)\\s*\\{"
        "[^}]*?"  // Non-greedy match of anything
        "for\\s*\\([^{]*\\)\\s*\\{"
        "[^}]*?"  // Non-greedy match of anything
        "for\\s*\\([^{]*\\)\\s*\\{"
        "[^}]*?"  // Non-greedy match of anything
        "C\\[.*?\\].*?\\+=.*?A\\[.*?\\].*?\\*.*?B\\[.*?\\]"
        "[^}]*?"  // Non-greedy match of anything
        "\\}\\s*\\}\\s*\\}"
    );
    
    std::sregex_iterator it(code.begin(), code.end(), matrixMultPattern);
    std::sregex_iterator end;
    
    while (it != end) {
        std::smatch match = *it;
        patterns["matrix_multiplication"] = match.str();
        ++it;
    }
    
    // Find large fixed-size matrix patterns
    std::regex fixedMatrixPattern(
        "void\\s+matrix\\w+\\([^)]*\\[\\d+\\][^)]*\\)\\s*\\{"
    );
    
    it = std::sregex_iterator(code.begin(), code.end(), fixedMatrixPattern);
    
    while (it != end) {
        std::smatch match = *it;
        // Find the entire function body
        std::string funcStart = match.str();
        size_t startPos = code.find(funcStart);
        if (startPos != std::string::npos) {
            int braceCount = 0;
            size_t pos = startPos + funcStart.length() - 1; // Start at the opening brace
            do {
                if (code[pos] == '{') braceCount++;
                else if (code[pos] == '}') braceCount--;
                pos++;
            } while (braceCount > 0 && pos < code.length());
            
            patterns["fixed_size_matrix"] = code.substr(startPos, pos - startPos);
        }
        ++it;
    }
    
    return patterns;
}

std::map<std::string, std::vector<PIMInstruction>> RefactoringAssistant::findInstructionPatterns(
    const std::vector<PIMInstruction>& instructions) {
    
    std::map<std::string, std::vector<PIMInstruction>> patterns;
    
    // Check for redundant load operations
    std::vector<PIMInstruction> redundantLoads;
    std::map<std::string, int> loadCount;
    
    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& instr = instructions[i];
        
        if (instr.getOpcode() == PIM_LOAD) {
            std::string source = std::to_string(instr.getSrc1());
            loadCount[source]++;
            
            if (loadCount[source] > 1) {
                // Find a sequence of redundant loads (up to 3 instructions)
                size_t endIdx = std::min(i + 3, instructions.size());
                redundantLoads.insert(redundantLoads.end(), 
                                      instructions.begin() + i, 
                                      instructions.begin() + endIdx);
                break;
            }
        }
    }
    
    if (!redundantLoads.empty()) {
        patterns["redundant_loads"] = redundantLoads;
    }
    
    // Check for inefficient accumulation patterns
    std::vector<PIMInstruction> inefficientAccum;
    for (size_t i = 0; i < instructions.size() - 3; ++i) {
        const auto& instr1 = instructions[i];
        const auto& instr2 = instructions[i+1];
        const auto& instr3 = instructions[i+2];
        
        // Look for MUL followed by ADD with store and then immediate reload
        if (instr1.getOpcode() == PIM_MUL && 
            instr2.getOpcode() == PIM_ADD && 
            instr3.getOpcode() == PIM_STORE) {
            
            // Check if there's a LOAD of the same value shortly after
            for (size_t j = i + 3; j < std::min(i + 6, instructions.size()); ++j) {
                if (instructions[j].getOpcode() == PIM_LOAD && 
                    instructions[j].getSrc1() == instr3.getDest()) {
                    
                    inefficientAccum.insert(inefficientAccum.end(),
                                           instructions.begin() + i,
                                           instructions.begin() + j + 1);
                    break;
                }
            }
            
            if (!inefficientAccum.empty()) {
                break;
            }
        }
    }
    
    if (!inefficientAccum.empty()) {
        patterns["inefficient_accumulation"] = inefficientAccum;
    }
    
    return patterns;
}

