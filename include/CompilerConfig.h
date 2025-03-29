/**
 * CompilerConfig.h
 * Configuration parameters for the PIM compiler
 */

#ifndef COMPILER_CONFIG_H
#define COMPILER_CONFIG_H

#include <string>

/**
 * Compiler configuration settings
 */
struct CompilerConfig {
    // Optimization level
    enum OptimizationLevel {
        O0 = 0,  // No optimization
        O1,      // Basic optimization
        O2,      // Medium optimization (default)
        O3       // Aggressive optimization
    };
    
    // PIM Architecture parameters
    struct PIMArchParams {
        unsigned numProcessingElements = 128;  // Number of processing elements
        unsigned memoryBankSize = 1024;        // Size of each memory bank in bytes
        unsigned numMemoryBanks = 64;          // Number of memory banks
        unsigned registerFileSize = 8;         // Number of registers per PE
        unsigned wordSize = 32;                // Word size in bits
        unsigned matrixDimLimit = 1024;        // Maximum supported matrix dimension
    };
    
    // Default configuration
    static CompilerConfig getDefaultConfig() {
        CompilerConfig config;
        config.optimizationLevel = O2;
        config.outputFormat = "text";
        config.verboseOutput = false;
        config.enableMemoryMapping = true;
        return config;
    }
    
    // Configuration parameters
    OptimizationLevel optimizationLevel = O2;
    std::string outputFormat = "text";         // "text" or "binary"
    bool verboseOutput = false;
    bool enableMemoryMapping = true;
    PIMArchParams archParams;
};

#endif // COMPILER_CONFIG_H
