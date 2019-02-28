#ifndef ASTToken_HPP
    #define ASTToken_HPP
#include "SymbolTable.hpp"
#include <memory>
#include <vector>
#include <string>

namespace Cminus { namespace Structures
{
    enum class ASTOperationType
    {
        Literal = 0,
        ArrayAccess,
        Negation,
        BinaryNot,
        LogicalNot,
        Exponentiation,
        Modulo,
        Divide,
        Multiply,
        Add,
        Subtract,
        RightShift,
        LeftShift,
        BinaryXor,
        BinaryOr,
        LogicalOr,
        BinaryAnd,
        LogicalAnd,
        TestGT,
        TestGE,
        TestLT,
        TestLE,
        TestNE,
        TestEQ,
        Assign,
    };

    typedef int TypeID;

    class ASTNode
    {

    };

    class ExpressionASTNode : public ASTNode
    {
        public:
            ExpressionASTNode();
            ExpressionASTNode(TypeID type);
            TypeID Type;
            bool IsReference;
    };

    class BinaryOperationASTNode : public ExpressionASTNode
    {
        public:
            BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs);
            ASTOperationType Operation;
            ExpressionASTNode* LeftSide;
            ExpressionASTNode* RightSide;
    };

    class UnaryOperationASTNode : public ExpressionASTNode
    {
        public:
            UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member);
            ASTOperationType Operation;
            ExpressionASTNode* Member;
    };

    template <typename ValueType>
    class ScalarLiteralASTNode : public ExpressionASTNode
    {
        public:
            ScalarLiteralASTNode(TypeID typeID, ValueType value);
            ValueType Value;
    };

    template <typename ValueType>
    ScalarLiteralASTNode<ValueType>::ScalarLiteralASTNode(TypeID typeID, ValueType value)
        : ExpressionASTNode(typeID), Value(value)
    {
        // nop
    }

    class ArrayLiteralASTNode : public ExpressionASTNode
    {
        public:
            ArrayLiteralASTNode(TypeID memberTypeID);
            std::vector<ExpressionASTNode*> Members;
    };

    class VariableASTNode : public ExpressionASTNode
    {
        public:
            VariableASTNode(std::string id);
            std::string ID;
    };

    class FunctionCallASTNode : public ExpressionASTNode
    {
        public:
            FunctionCallASTNode(std::string id);
            std::string ID;
            std::vector<ExpressionASTNode*> Arguments;
    };

    class ExpressionListASTNode : public ExpressionASTNode
    {
        public:
            ExpressionListASTNode();
            std::vector<ExpressionASTNode*> Members;
    };

    class StatementListASTNode : public ASTNode
    {
        public:
            std::vector<ASTNode*> Members;
    };

    class SimpleDeclarationASTNode
    {
        public:
            SimpleDeclarationASTNode(TypeID typeID, std::string id);
            TypeID Type;
            std::string ID;
    };

    class FunctionDeclarationASTNode : public ASTNode
    {
        public:
            FunctionDeclarationASTNode(std::string id, StatementListASTNode* body);
            TypeID Type;
            std::string ID;
            std::vector<SimpleDeclarationASTNode*> Arguments;
            StatementListASTNode* Body;
    };

    class SingleVariableDeclarationASTNode
    {
        public:
            SingleVariableDeclarationASTNode(std::string id, bool isArray, ScalarLiteralASTNode<int>* arraySize);
            SingleVariableDeclarationASTNode(std::string id, bool isArray, ScalarLiteralASTNode<int>* arraySize, ExpressionASTNode* initialValue);
            std::string ID;
            ExpressionASTNode* InitialValue;
            bool IsArray;
            ScalarLiteralASTNode<int>* ArraySize;
    };

    class VariableDeclarationASTNode : public ASTNode
    {
        public:
            TypeID Type;
            std::vector<SingleVariableDeclarationASTNode*> Members;
    };

    class ExitStatementASTNode : public ASTNode
    {
    };

    class BreakStatementASTNode : public ASTNode
    {
    };

    class ReturnStatementASTNode : public ASTNode
    {
        public:
            ReturnStatementASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;
    };

    class WhileStatementASTNode : public ASTNode
    {
        public:
            WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body);
            ExpressionASTNode* Test;
            ASTNode* Body;
    };

    class IfStatementASTNode : public ASTNode
    {
        public:
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue);
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse);
            ExpressionASTNode* Test;
            ASTNode* IfTrue;
            ASTNode* IfFalse;
    };

    class ReadCallASTNode : public ASTNode
    {
        public:
            ReadCallASTNode(ExpressionASTNode* variable);
            ExpressionASTNode* Variable;
    };

    class WriteCallASTNode : public ASTNode
    {
        public:
            WriteCallASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;
    };
}}

#endif
