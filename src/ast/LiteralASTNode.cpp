#include "LiteralASTNode.hpp"
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

    ASTNode* IntegerLiteralASTNode::Check(DriverState& state)
    {
        return this;
    }

    ASTNode* StringLiteralASTNode::Check(DriverState& state)
    {
        auto id = state.AddStringConstant(Value);
        Value = std::string(".string_constant").append(to_string(id));
        return this;
    }

    void ArrayLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        // TODO
    }

    void IntegerLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "\tmov " << destinationRegister << ", " << Value << endl;
    }

    void StringLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "\tlea " << destinationRegister << ", " << Value << "[rip]" << endl;
    }
}}
