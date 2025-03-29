/**
 * Parser.h
 * Parses C++ matrix multiplication code and converts it to an AST
 */

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <vector>

// Define this in CMakeLists.txt
#ifdef HAVE_CLANG
#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#endif

// Forward declarations for Clang classes
#ifndef HAVE_CLANG
namespace clang {
    class ASTContext;
    class TranslationUnitDecl;
}
#endif

class ParserASTConsumer;
class ParserAction;

class Parser {
public:
    Parser();
    ~Parser();

    /**
     * Parse C++ source code and return the AST
     * 
     * @param source C++ source code as string
     * @return AST context
     * @throws std::runtime_error if parsing fails
     */
#ifdef HAVE_CLANG
    std::unique_ptr<clang::ASTContext> parse(const std::string& source);
#else
    void* parse(const std::string& source);
#endif

    /**
     * Get the compiled translation unit
     */
#ifdef HAVE_CLANG
    clang::TranslationUnitDecl* getTranslationUnit() const;
#endif

private:
#ifdef HAVE_CLANG
    std::unique_ptr<clang::ASTContext> astContext;
    clang::TranslationUnitDecl* translationUnit;
#endif
    
    friend class ParserASTConsumer;
    friend class ParserAction;
};

#endif // PARSER_H
