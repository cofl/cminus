#ifndef IfStatementASTNode_HPP
    #define IfStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class IfStatementASTNode : public ASTNode
    {
        public:
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue);
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse);
            ExpressionASTNode* Test;
            ASTNode* IfTrue;
            ASTNode* IfFalse;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
