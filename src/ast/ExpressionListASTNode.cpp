#include "ExpressionListASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    ExpressionListASTNode::ExpressionListASTNode()
        :   ExpressionASTNode(ASTNodeType::FunctionDeclaration)
    {
        // nop
    }

    ASTNode* ExpressionListASTNode::Check(DriverState& state)
    {
        int len = Members.size();
        for(int i = 0; i < len; i += 1)
        {
            auto member = Members[i];
            member->Symbols = this->Symbols;
            Members[i] = (ExpressionASTNode*) member->Check(state);
        }
        this->Type = Members.back()->Type;
        return this;
    }

    void ExpressionListASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        for(auto const& member: Members)
            member->Emit(state, destinationRegister);
    }
}}
