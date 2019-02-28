#include "ASTToken.hpp"
#include "SymbolTable.hpp"

namespace Cminus { namespace Structures
{
    WriteCallASTNode::WriteCallASTNode(ExpressionASTNode* value)
        : Value(value) {}

    ReadCallASTNode::ReadCallASTNode(ExpressionASTNode* variable)
        : Variable(variable) {}

    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue)
        : Test(test), IfTrue(ifTrue), IfFalse(nullptr) {}
    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse)
        : Test(test), IfTrue(ifTrue), IfFalse(ifFalse) {}

    WhileStatementASTNode::WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body)
        : Test(test), Body(body) {}

    ReturnStatementASTNode::ReturnStatementASTNode(ExpressionASTNode* value)
        : Value(value) {}

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, bool isArray, ScalarLiteralASTNode<int>* arraySize)
        : ID(id), IsArray(isArray), ArraySize(arraySize), InitialValue(nullptr) {}
    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, bool isArray, ScalarLiteralASTNode<int>* arraySize, ExpressionASTNode* initialValue)
        : ID(id), IsArray(isArray), ArraySize(arraySize), InitialValue(initialValue) {}

    FunctionDeclarationASTNode::FunctionDeclarationASTNode(std::string id, StatementListASTNode* body)
        : ID(id), Body(body) {}

    ExpressionListASTNode::ExpressionListASTNode()
        : ExpressionASTNode()
    {
        // TODO: type
    }

    FunctionCallASTNode::FunctionCallASTNode(std::string id)
        : ExpressionASTNode(), ID(id)
    {
        // TODO: type
    }

    VariableASTNode::VariableASTNode(std::string id)
        : ExpressionASTNode(), ID(id)
    {
        IsReference = true;
        // TODO: type
    }

    ArrayLiteralASTNode::ArrayLiteralASTNode(TypeID memberTypeID)
        : ExpressionASTNode(memberTypeID) {}

    UnaryOperationASTNode::UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member)
        : ExpressionASTNode(), Operation(operation), Member(member)
    {
        // TODO: type
    }

    BinaryOperationASTNode::BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs)
        : ExpressionASTNode(), Operation(operation), LeftSide(lhs), RightSide(rhs)
    {
        // TODO: type
    }

    ExpressionASTNode::ExpressionASTNode() {}
    ExpressionASTNode::ExpressionASTNode(TypeID type)
        : Type(type) {}
}}
