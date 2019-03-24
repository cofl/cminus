#ifndef IfStatementASTNode_HPP
    #define IfStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

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

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
