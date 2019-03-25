#ifndef BinaryOperationASTNode_HPP
    #define BinaryOperationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class BinaryOperationASTNode : public ExpressionASTNode
    {
        public:
            BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs);
            ASTOperationType Operation;
            ExpressionASTNode* LeftSide;
            ExpressionASTNode* RightSide;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
    };
}}

#endif
