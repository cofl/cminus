#include "LiteralASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    ArrayLiteralASTNode::ArrayLiteralASTNode(int memberTypeID)
        :   ExpressionASTNode(ASTNodeType::ArrayLiteral, memberTypeID)
    {
        // nop
    }

    IntegerLiteralASTNode::IntegerLiteralASTNode(int typeID, int value)
        :   ExpressionASTNode(ASTNodeType::IntegerLiteral, typeID),
            Value(value)
    {
        IsConstant = true;
    }

    StringLiteralASTNode::StringLiteralASTNode(int typeID, string value)
        :   ExpressionASTNode(ASTNodeType::StringLiteral, typeID),
            Value(value)
    {
        IsConstant = true;
    }

    ASTNode* IntegerLiteralASTNode::Check(State& state)
    {
        return this;
    }

    ASTNode* StringLiteralASTNode::Check(State& state)
    {
        Value = state.InternStringLiteral(Value);
        return this;
    }

    void ArrayLiteralASTNode::Emit(State& state, Register& destination)
    {
        // TODO
    }

    void IntegerLiteralASTNode::Emit(State& state, Register& destination)
    {
        ASM::Move(state, destination, Value);
    }

    void StringLiteralASTNode::Emit(State& state, Register& destination)
    {
        ASM::LoadGlobalAddress(state, destination, Value);
    }
}}
