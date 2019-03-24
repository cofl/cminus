#ifndef ForStatementASTNode_HPP
    #define ForStatementASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

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

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
