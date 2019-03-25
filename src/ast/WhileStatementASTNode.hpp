#ifndef WhileStatementASTNode_HPP
    #define WhileStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class WhileStatementASTNode : public ASTNode
    {
        public:
            WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body);
            ExpressionASTNode* Test;
            ASTNode* Body;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
