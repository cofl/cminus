#ifndef ExpressionASTNode_HPP
    #define ExpressionASTNode_HPP

#include "ASTNode.hpp"

namespace Cminus { namespace AST
{
    class ExpressionASTNode : public ASTNode
    {
        public:
            ExpressionASTNode(ASTNodeType nodeType);
            ExpressionASTNode(ASTNodeType nodeType, int type);
            int Type;
            bool IsLValue;
            bool IsConstant;

            void Emit(State& state);
            virtual void Emit(State& state, Register& destination) = 0;
            virtual void EmitLValue(State& state, Register& destination);
    };
}}

#endif
