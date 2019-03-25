#ifndef ForStatementASTNode_HPP
    #define ForStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class ForStatementASTNode : public ASTNode
    {
        public:
            ForStatementASTNode(ExpressionASTNode* initial, ExpressionASTNode* test, ExpressionASTNode* step, ASTNode* body);
            ExpressionASTNode* Initial;
            ExpressionASTNode* Test;
            ExpressionASTNode* Step;
            ASTNode* Body;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
