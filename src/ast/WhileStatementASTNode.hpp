#ifndef WhileStatementASTNode_HPP
    #define WhileStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class WhileStatementASTNode : public ASTNode
    {
        public:
            WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body);
            ExpressionASTNode* Test;
            ASTNode* Body;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
