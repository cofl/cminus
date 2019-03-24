#ifndef VariableDeclarationASTNode_HPP
    #define VariableDeclarationASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"
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

            void Emit(DriverState& state);
    };

    class VariableDeclarationASTNode : public ASTNode
    {
        public:
            VariableDeclarationASTNode();
            int Type;
            std::vector<SingleVariableDeclarationASTNode*> Members;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
