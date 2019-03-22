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

    ForStatementASTNode::ForStatementASTNode(ExpressionASTNode* initial, ExpressionASTNode* test, ExpressionASTNode* step, ASTNode* body)
        : ASTNode(ASTNodeType::For), Initial(initial), Test(test), Step(step), Body(body) {}

    ReturnStatementASTNode::ReturnStatementASTNode(ExpressionASTNode* value)
        : ASTNode(ASTNodeType::Return), Value(value) {}

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, int arraySize)
        : ID(id), ArraySize(arraySize), InitialValue(nullptr) {}

    SingleVariableDeclarationASTNode::SingleVariableDeclarationASTNode(std::string id, int arraySize, ExpressionASTNode* initialValue)
        : ID(id), ArraySize(arraySize), InitialValue(initialValue) {}

    FunctionDeclarationASTNode::FunctionDeclarationASTNode(std::string id, StatementListASTNode* body)
        : ASTNode(ASTNodeType::FunctionDeclaration), ID(id), Body(body), Arguments(nullptr) {}

    ExpressionListASTNode::ExpressionListASTNode()
        : ExpressionASTNode(ASTNodeType::FunctionDeclaration)
    {
        // TODO: type
    }

    FunctionCallASTNode::FunctionCallASTNode(std::string id)
        : ExpressionASTNode(ASTNodeType::FunctionCall), ID(id), Arguments()
    {
        // TODO: type
    }

    FunctionCallASTNode::FunctionCallASTNode(std::string id, ExpressionListASTNode* args)
        : ExpressionASTNode(ASTNodeType::FunctionCall), ID(id)
    {
        Arguments = args->Members;
    }

    VariableASTNode::VariableASTNode(std::string id)
        : ExpressionASTNode(ASTNodeType::Variable), ID(id), ArrayIndex(nullptr)
    {
        IsLValue = true;
        // TODO: type
    }

    VariableASTNode::VariableASTNode(std::string id, ExpressionASTNode* arrayIndex)
        : ExpressionASTNode(ASTNodeType::Variable), ID(id), ArrayIndex(arrayIndex)
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

    NopStatementASTNode::NopStatementASTNode()
        : ASTNode(ASTNodeType::Nop) {}

    ExitStatementASTNode::ExitStatementASTNode()
        : ASTNode(ASTNodeType::Exit) {}

    BreakStatementASTNode::BreakStatementASTNode()
        : ASTNode(ASTNodeType::Break) {}

    ContinueStatementASTNode::ContinueStatementASTNode()
        : ASTNode(ASTNodeType::Continue) {}

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

    ASMASTNode::ASMASTNode(string value)
        : ASTNode(ASTNodeType::ASM), Value(value)
    {
        // nop
    }

    SimpleDeclarationASTNode::SimpleDeclarationASTNode(int typeID, string id)
        : Type(typeID), ID(id)
    {
        // nop
    }
}}
