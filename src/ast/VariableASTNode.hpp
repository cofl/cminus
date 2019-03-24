#ifndef VariableASTNode_HPP
    #define VariableASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class VariableASTNode : public ExpressionASTNode
    {
        public:
            VariableASTNode(std::string id);
            VariableASTNode(std::string id, ExpressionASTNode* index);
            std::string ID;
            ExpressionASTNode* ArrayIndex;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
            void EmitLValue(DriverState& state, const char* destinationRegister);
    };
}}

#endif
