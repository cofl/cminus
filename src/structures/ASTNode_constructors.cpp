#include "ASTNode.hpp"
#include "SymbolTable.hpp"

namespace Cminus { namespace Structures
{
    ASTNode::ASTNode(ASTNodeType nodeType)
        : NodeType(nodeType), Symbols(nullptr) {}

    ExpressionASTNode::ExpressionASTNode(ASTNodeType nodeType)
        : ASTNode(nodeType), IsConstant(false), IsLValue(false) {}
    ExpressionASTNode::ExpressionASTNode(ASTNodeType nodeType, int type)
        : ASTNode(nodeType), IsConstant(false), IsLValue(false), Type(type) {}

    WriteCallASTNode::WriteCallASTNode(ExpressionASTNode* value)
        : ASTNode(ASTNodeType::WriteCall), Value(value) {}

    ReadCallASTNode::ReadCallASTNode(ExpressionASTNode* variable)
        : ASTNode(ASTNodeType::ReadCall), Variable(variable) {}

    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue)
        : ASTNode(ASTNodeType::If), Test(test), IfTrue(ifTrue), IfFalse(nullptr) {}

    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse)
        : ASTNode(ASTNodeType::If), Test(test), IfTrue(ifTrue), IfFalse(ifFalse) {}

    WhileStatementASTNode::WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body)
        : ASTNode(ASTNodeType::While), Test(test), Body(body) {}

    ReturnStatementASTNode::ReturnStatementASTNode(ExpressionASTNode* value)
        : ASTNode(ASTNodeType::Return), Value(value) {}

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, bool isArray, IntegerLiteralASTNode* arraySize)
        : ID(id), IsArray(isArray), ArraySize(arraySize), InitialValue(nullptr) {}

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, bool isArray, IntegerLiteralASTNode* arraySize, ExpressionASTNode* initialValue)
        : ID(id), IsArray(isArray), ArraySize(arraySize), InitialValue(initialValue) {}

    FunctionDeclarationASTNode::FunctionDeclarationASTNode(std::string id, StatementListASTNode* body)
        : ASTNode(ASTNodeType::FunctionDeclaration), ID(id), Body(body) {}

    ExpressionListASTNode::ExpressionListASTNode()
        : ExpressionASTNode(ASTNodeType::FunctionDeclaration)
    {
        // TODO: type
    }

    FunctionCallASTNode::FunctionCallASTNode(std::string id)
        : ExpressionASTNode(ASTNodeType::FunctionCall), ID(id)
    {
        // TODO: type
    }

    VariableASTNode::VariableASTNode(std::string id)
        : ExpressionASTNode(ASTNodeType::Variable), ID(id)
    {
        IsLValue = true;
        // TODO: type
    }

    ArrayLiteralASTNode::ArrayLiteralASTNode(int memberTypeID)
        : ExpressionASTNode(ASTNodeType::ArrayLiteral, memberTypeID) {}

    UnaryOperationASTNode::UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member)
        : ExpressionASTNode(ASTNodeType::UnaryOperation), Operation(operation), Member(member)
    {
        // TODO: type
    }

    BinaryOperationASTNode::BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs)
        : ExpressionASTNode(ASTNodeType::BinaryOperation), Operation(operation), LeftSide(lhs), RightSide(rhs)
    {
        // TODO: type
    }

    StatementListASTNode::StatementListASTNode()
        : ASTNode(ASTNodeType::StatementList), IsGlobal(false) {}

    VariableDeclarationASTNode::VariableDeclarationASTNode()
        : ASTNode(ASTNodeType::VariableDeclaration) {}

    ExitStatementASTNode::ExitStatementASTNode()
        : ASTNode(ASTNodeType::Exit) {}

    BreakStatementASTNode::BreakStatementASTNode()
        : ASTNode(ASTNodeType::Break) {}

    IntegerLiteralASTNode::IntegerLiteralASTNode(int typeID, int value)
        : ExpressionASTNode(ASTNodeType::IntegerLiteral, typeID), Value(value)
    {
        IsConstant = true;
    }

    StringLiteralASTNode::StringLiteralASTNode(int typeID, string value)
        : ExpressionASTNode(ASTNodeType::StringLiteral, typeID), Value(value)
    {
        IsConstant = true;
    }
}}
