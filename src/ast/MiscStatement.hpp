#ifndef MiscStatement_HPP
    #define MiscStatement_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class BreakStatementASTNode : public ASTNode
    {
        public:
            BreakStatementASTNode();

            ASTNode* Check(State& state);
            void Emit(State& state);
    };

    class ContinueStatementASTNode : public ASTNode
    {
        public:
            ContinueStatementASTNode();

            ASTNode* Check(State& state);
            void Emit(State& state);
    };

    class ExitStatementASTNode : public ASTNode
    {
        public:
            ExitStatementASTNode();

            void Emit(State& state);
    };

    class NopStatementASTNode : public ASTNode
    {
        public:
            NopStatementASTNode();

            void Emit(State& state);
    };

    class ReturnStatementASTNode : public ASTNode
    {
        public:
            ReturnStatementASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
