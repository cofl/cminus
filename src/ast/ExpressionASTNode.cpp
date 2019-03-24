#include "ExpressionASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    ExpressionASTNode::ExpressionASTNode(ASTNodeType nodeType)
        :   ASTNode(nodeType),
            IsConstant(false),
            IsLValue(false)
    {
        // nop
    }

    ExpressionASTNode::ExpressionASTNode(ASTNodeType nodeType, int type)
        :   ASTNode(nodeType),
            IsConstant(false),
            IsLValue(false),
            Type(type)
    {
        // nop
    }

    void ExpressionASTNode::Emit(DriverState& state)
    {
        auto index = state.GetFreeRegister();
        Emit(state, state.RegisterNames32[index]);
        state.ReleaseRegister(index);
    }

    void ExpressionASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        // TODO: better error handling
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }

    void ExpressionASTNode::EmitLValue(DriverState& state, const char* destinationRegister)
    {
        // TODO: better error handling
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }
}}
