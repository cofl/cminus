#ifndef UnaryOperationASTNode_HPP
    #define UnaryOperationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class UnaryOperationASTNode : public ExpressionASTNode
    {
        public:
            UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member);
            ASTOperationType Operation;
            ExpressionASTNode* Member;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };
}}

#endif
