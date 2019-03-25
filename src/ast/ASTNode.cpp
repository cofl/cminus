#include "ASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    ASTNode::ASTNode(ASTNodeType nodeType)
        :   NodeType(nodeType),
            Symbols(nullptr)
    {
        // nop
    }

    ASTNode* ASTNode::Check(State& state)
    {
        // do-nothing as a default
        return this;
    }

    void ASTNode::Emit(State& state)
    {
        // TODO: better error handling
        state.OutputStream << "{{Unknown Node}}" << std::endl;
    }
}}
