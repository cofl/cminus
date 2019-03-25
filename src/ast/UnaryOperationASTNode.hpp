#ifndef UnaryOperationASTNode_HPP
    #define UnaryOperationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class UnaryOperationASTNode : public ExpressionASTNode
    {
        public:
            UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member);
            ASTOperationType Operation;
            ExpressionASTNode* Member;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
    };
}}

#endif
