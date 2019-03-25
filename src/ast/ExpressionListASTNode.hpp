#ifndef ExpressionListASTNode_HPP
    #define ExpressionListASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class ExpressionListASTNode : public ExpressionASTNode
    {
        public:
            ExpressionListASTNode();
            std::vector<ExpressionASTNode*> Members;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destinationRegister);
    };
}}

#endif
