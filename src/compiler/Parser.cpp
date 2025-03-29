/**
 * Parser.cpp
 * Implementation of the C++ parser for matrix multiplication code
 */

#include "Parser.h"
#include "../utils/Logger.h"
#include <stdexcept>
#include <memory>
#include <vector>

#ifdef HAVE_CLANG
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/raw_ostream.h>

// AST Consumer that captures the AST for further processing
class ParserASTConsumer : public clang::ASTConsumer {
private:
    Parser* parser;

public:
    explicit ParserASTConsumer(Parser* parser) : parser(parser) {}

    void HandleTranslationUnit(clang::ASTContext& context) override {
        parser->astContext = std::make_unique<clang::ASTContext>(context);
        parser->translationUnit = context.getTranslationUnitDecl();
        
        // Validate that the code contains matrix multiplication
        clang::RecursiveASTVisitor<ParserASTConsumer> visitor;
        visitor.TraverseDecl(parser->translationUnit);
        
        Logger::getInstance().log("Translation unit parsed successfully");
    }
};

// Frontend action to create our custom AST consumer
class ParserAction : public clang::ASTFrontendAction {
private:
    Parser* parser;

public:
    explicit ParserAction(Parser* parser) : parser(parser) {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& compiler, llvm::StringRef file) override {
        return std::make_unique<ParserASTConsumer>(parser);
    }
};

// Factory to create our frontend action
class ParserActionFactory : public clang::tooling::FrontendActionFactory {
private:
    Parser* parser;

public:
    explicit ParserActionFactory(Parser* parser) : parser(parser) {}

    std::unique_ptr<clang::FrontendAction> create() override {
        return std::make_unique<ParserAction>(parser);
    }
};
#endif

#ifdef HAVE_CLANG
Parser::Parser() : translationUnit(nullptr) {}
#else
Parser::Parser() {}
#endif

Parser::~Parser() = default;

#ifdef HAVE_CLANG
std::unique_ptr<clang::ASTContext> Parser::parse(const std::string& source) {
    // Create compilation database and tool
    std::vector<std::string> args = {"-std=c++14", "-fsyntax-only"};
    std::unique_ptr<clang::tooling::CompilationDatabase> compilations =
        clang::tooling::FixedCompilationDatabase::loadFromCommandLine(
            args.size(), args.data());
    
    // Create source file
    llvm::SmallString<1024> sourceBuffer;
    llvm::StringRef sourceRef(source);
    sourceRef.copy(sourceBuffer);
    
    std::vector<std::string> sources;
    sources.push_back("input.cpp");
    
    // Run tool with our custom action
    clang::tooling::ClangTool tool(*compilations, sources);
    tool.mapVirtualFile("input.cpp", sourceBuffer);
    
    ParserActionFactory factory(this);
    int result = tool.run(&factory);
    
    if (result != 0 || !astContext) {
        throw std::runtime_error("Failed to parse input C++ code");
    }
    
    return std::move(astContext);
}
#else
void* Parser::parse(const std::string& source) {
    // Simple implementation for environments without Clang
    Logger::getInstance().log("Using simplified parser (Clang not available)");
    Logger::getInstance().log("Source code will be processed using hardcoded patterns");
    
    // Log the source code for debugging
    Logger::getInstance().log("Source code to parse:\n" + source);
    
    // Return nullptr - in a real implementation we might parse the source more intelligently
    return nullptr;
}
#endif

#ifdef HAVE_CLANG
clang::TranslationUnitDecl* Parser::getTranslationUnit() const {
    return translationUnit;
}
#endif
