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

    void ExpressionASTNode::Emit(State& state)
    {
        auto reg = state.AllocRegister(RegisterLength::_32);
        Emit(state, reg);
        state.FreeRegister(reg);
    }

    void ExpressionASTNode::Emit(State& state, Register& destination)
    {
        // TODO: better error handling
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }

    void ExpressionASTNode::EmitLValue(State& state, Register& destination)
    {
        // TODO: better error handling
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }
}}
