#ifndef MiscStatement_HPP
    #define MiscStatement_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class BreakStatementASTNode : public ASTNode
    {
        public:
            BreakStatementASTNode();

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ContinueStatementASTNode : public ASTNode
    {
        public:
            ContinueStatementASTNode();

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ExitStatementASTNode : public ASTNode
    {
        public:
            ExitStatementASTNode();

            void Emit(DriverState& state);
    };

    class NopStatementASTNode : public ASTNode
    {
        public:
            NopStatementASTNode();

            void Emit(DriverState& state);
    };

    class ReturnStatementASTNode : public ASTNode
    {
        public:
            ReturnStatementASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
