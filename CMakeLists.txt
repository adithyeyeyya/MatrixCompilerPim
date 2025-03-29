cmake_minimum_required(VERSION 3.10)

project(PIMCompiler VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Clang might not be available as a separate package in some environments
# We'll check if it's available, and if not, we'll use alternatives
find_package(Clang CONFIG QUIET)
if(Clang_FOUND)
    message(STATUS "Found Clang ${CLANG_PACKAGE_VERSION}")
    message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")
    set(HAVE_CLANG TRUE)
    add_definitions(-DHAVE_CLANG)
else()
    message(STATUS "Clang development package not found, using workarounds")
    set(HAVE_CLANG FALSE)
endif()

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Include Clang directories if found
if(HAVE_CLANG)
    include_directories(${CLANG_INCLUDE_DIRS})
endif()

# Include project directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCE_FILES
    src/main.cpp
    src/compiler/Parser.cpp
    src/compiler/IRGenerator.cpp
    src/compiler/PIMBackend.cpp
    src/compiler/MemoryMapper.cpp
    src/compiler/PIMInstruction.cpp
    src/optimizer/RefactoringAssistant.cpp
    src/utils/Logger.cpp
)

# Header files
set(HEADER_FILES
    src/compiler/Parser.h
    src/compiler/IRGenerator.h
    src/compiler/PIMBackend.h
    src/compiler/MemoryMapper.h
    src/compiler/PIMInstruction.h
    src/optimizer/RefactoringAssistant.h
    src/utils/Logger.h
    include/PIMInstructionSet.h
    include/CompilerConfig.h
)

# Executable
add_executable(pim_compiler ${SOURCE_FILES} ${HEADER_FILES})

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs 
    Analysis
    Core
    ExecutionEngine
    InstCombine
    Object
    OrcJIT
    Support
    native
)

# Link libraries
if(HAVE_CLANG)
    # Link Clang libraries if available
    target_link_libraries(pim_compiler PRIVATE
        clangAST
        clangAnalysis
        clangBasic
        clangDriver
        clangEdit
        clangFrontend
        clangLex
        clangParse
        clangSema
        clangSerialization
        clangTooling
        ${llvm_libs}
    )
else()
    # Just link LLVM libraries if Clang is not available
    target_link_libraries(pim_compiler PRIVATE
        ${llvm_libs}
    )
endif()

# Add compile options
target_compile_options(pim_compiler PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Install target
install(TARGETS pim_compiler DESTINATION bin)

# Add testing
enable_testing()

# Copy test files to build directory
file(GLOB TEST_SCRIPTS "test/*.py")
foreach(TEST_SCRIPT ${TEST_SCRIPTS})
    file(COPY ${TEST_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/test)
    get_filename_component(TEST_NAME ${TEST_SCRIPT} NAME_WE)
    add_test(NAME ${TEST_NAME} COMMAND python3 ${CMAKE_BINARY_DIR}/test/${TEST_NAME}.py)
endforeach()

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary")
message(STATUS "---------------------")
message(STATUS "C++ Compiler:     ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Flags:        ${CMAKE_CXX_FLAGS}")
message(STATUS "Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "LLVM version:     ${LLVM_PACKAGE_VERSION}")
message(STATUS "Clang version:    ${CLANG_PACKAGE_VERSION}")
message(STATUS "")
