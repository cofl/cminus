#ifndef VariableASTNode_HPP
    #define VariableASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class VariableASTNode : public ExpressionASTNode
    {
        public:
            VariableASTNode(std::string id);
            VariableASTNode(std::string id, ExpressionASTNode* index);
            std::string ID;
            ExpressionASTNode* ArrayIndex;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
            void EmitLValue(State& state, Register& destination);
    };
}}

#endif
