#ifndef ExpressionASTNode_HPP
    #define ExpressionASTNode_HPP

#include "ASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

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

            void Emit(DriverState& state);
            virtual void Emit(DriverState& state, const char* destinationRegister);
            virtual void EmitLValue(DriverState& state, const char* destinationRegister);
    };
}}

#endif
