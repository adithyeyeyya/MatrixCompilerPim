#ifndef REFACTORING_ASSISTANT_H
#define REFACTORING_ASSISTANT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "utils/Logger.h"
#include "compiler/PIMInstruction.h"

/**
 * @class RefactoringRule
 * @brief Base class for refactoring rules
 */
class RefactoringRule {
public:
    virtual ~RefactoringRule() = default;
    
    /**
     * @brief Check if the rule applies to the given code
     * @param code The source code to check
     * @return True if the rule applies, false otherwise
     */
    virtual bool applies(const std::string& code) = 0;
    
    /**
     * @brief Apply the refactoring rule to the given code
     * @param code The source code to refactor
     * @return The refactored code
     */
    virtual std::string apply(const std::string& code) = 0;
    
    /**
     * @brief Get a description of the rule
     * @return A description of the rule
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Get the expected performance improvement
     * @return A string describing the expected performance improvement
     */
    virtual std::string getPerformanceImpact() const = 0;
};

/**
 * @class LoopReorderingRule
 * @brief Rule for reordering loops to optimize for PIM architecture
 */
class LoopReorderingRule : public RefactoringRule {
public:
    bool applies(const std::string& code) override;
    std::string apply(const std::string& code) override;
    std::string getDescription() const override;
    std::string getPerformanceImpact() const override;
};

/**
 * @class MatrixLayoutRule
 * @brief Rule for optimizing matrix layout for PIM architecture
 */
class MatrixLayoutRule : public RefactoringRule {
public:
    bool applies(const std::string& code) override;
    std::string apply(const std::string& code) override;
    std::string getDescription() const override;
    std::string getPerformanceImpact() const override;
};

/**
 * @class BlockingOptimizationRule
 * @brief Rule for applying blocking optimizations to matrix operations
 */
class BlockingOptimizationRule : public RefactoringRule {
public:
    bool applies(const std::string& code) override;
    std::string apply(const std::string& code) override;
    std::string getDescription() const override;
    std::string getPerformanceImpact() const override;
};

/**
 * @class RefactoringAssistant
 * @brief AI-powered assistant for refactoring code for PIM architecture
 */
class RefactoringAssistant {
public:
    RefactoringAssistant();
    ~RefactoringAssistant();
    
    /**
     * @brief Analyze code and suggest refactorings
     * @param sourceCode The source code to analyze
     * @return A map of refactoring suggestions, with keys being rule names and
     *         values being pairs of original code and refactored code
     */
    std::map<std::string, std::pair<std::string, std::string>> suggestRefactorings(
        const std::string& sourceCode);
    
    /**
     * @brief Analyze the PIM instructions and suggest optimizations
     * @param instructions The PIM instructions to analyze
     * @param sourceCode The original source code
     * @return A map of optimization suggestions
     */
    std::map<std::string, std::string> suggestInstructionOptimizations(
        const std::vector<PIMInstruction>& instructions,
        const std::string& sourceCode);
    
    /**
     * @brief Set verbosity level for the assistant
     * @param level The verbosity level
     */
    void setVerbosity(int level);
    
private:
    std::vector<std::unique_ptr<RefactoringRule>> rules;
    int verbosityLevel;
    
    /**
     * @brief Initialize the refactoring rules
     */
    void initializeRules();
    
    /**
     * @brief Find patterns in code that could be optimized
     * @param code The source code to analyze
     * @return A map of pattern names to code snippets
     */
    std::map<std::string, std::string> findPatterns(const std::string& code);
    
    /**
     * @brief Analyze instruction patterns for optimization opportunities
     * @param instructions The PIM instructions to analyze
     * @return A map of pattern names to instruction sequences
     */
    std::map<std::string, std::vector<PIMInstruction>> findInstructionPatterns(
        const std::vector<PIMInstruction>& instructions);
};

#endif // REFACTORING_ASSISTANT_H