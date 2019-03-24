#include "VariableDeclarationASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    VariableDeclarationASTNode::VariableDeclarationASTNode()
        :   ASTNode(ASTNodeType::VariableDeclaration)
    {
        // nop
    }

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, int arraySize)
        :   ID(id),
            ArraySize(arraySize),
            InitialValue(nullptr)
    {
        // nop
    }

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, int arraySize, ExpressionASTNode* initialValue)
        :   ID(id),
            ArraySize(arraySize),
            InitialValue(initialValue)
    {
        // nop
    }

    ASTNode* VariableDeclarationASTNode::Check(DriverState& state)
    {
        auto scope = state.SymbolStack.back();
        int len = Members.size();
        for(int i = 0; i < len; i += 1)
        {
            auto member = Members[i];
            auto data = scope->AddVariable(member->ID);
            data->Type = this->Type;
            data->InitialValue = nullptr;
            data->ArraySize = member->ArraySize;
            // TODO: initializer
        }
        return this;
    }

    void VariableDeclarationASTNode::Emit(DriverState& state)
    {
        // TODO?
    }
}}
