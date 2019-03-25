#ifndef VariableDeclarationASTNode_HPP
    #define VariableDeclarationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include <string>

namespace Cminus { namespace AST
{
    class SingleVariableDeclarationASTNode
    {
        public:
            SingleVariableDeclarationASTNode(std::string id, int arraySize);
            SingleVariableDeclarationASTNode(std::string id, int arraySize, ExpressionASTNode* initialValue);
            std::string ID;
            ExpressionASTNode* InitialValue;
            int ArraySize;

            void Emit(State& state);
    };

    class VariableDeclarationASTNode : public ASTNode
    {
        public:
            VariableDeclarationASTNode();
            int Type;
            std::vector<SingleVariableDeclarationASTNode*> Members;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
