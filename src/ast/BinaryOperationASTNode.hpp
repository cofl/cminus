#ifndef BinaryOperationASTNode_HPP
    #define BinaryOperationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class BinaryOperationASTNode : public ExpressionASTNode
    {
        public:
            BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs);
            ASTOperationType Operation;
            ExpressionASTNode* LeftSide;
            ExpressionASTNode* RightSide;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };
}}

#endif
