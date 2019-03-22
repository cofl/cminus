#ifndef ASTNode_HPP
    #define ASTNode_HPP

#include "SymbolTable.hpp"
#include "../DriverState.hpp"
#include <typeinfo>
#include <memory>
#include <vector>
#include <string>

namespace Cminus { namespace Structures
{
    enum class ASTOperationType
    {
        Literal,
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

    enum class ASTNodeType
    {
        StatementList,
        If,
        While,
        For,
        FunctionDeclaration,
        VariableDeclaration,
        IntegerLiteral,
        StringLiteral,
        ArrayLiteral,
        WriteCall,
        ReadCall,
        ExpressionList,
        FunctionCall,
        Variable,
        UnaryOperation,
        BinaryOperation,
        Return,
        Exit,
        Break,
        Continue,
        Nop,
        ASM
    };

    class ASTNode
    {
        public:
            ASTNode(ASTNodeType nodeType);
            ASTNodeType NodeType;
            SymbolTable* Symbols;

            virtual void Emit(DriverState& state);
            virtual ASTNode* Check(DriverState& state);
    };

    class ExpressionASTNode : public ASTNode
    {
        public:
            ExpressionASTNode(ASTNodeType nodeType);
            ExpressionASTNode(ASTNodeType nodeType, int type);
            int Type;
            bool IsLValue;
            bool IsConstant;

            void Emit(DriverState& state);
            virtual void Emit(DriverState& state, const char* destinationRegister);
            virtual void EmitLValue(DriverState& state, const char* destinationRegister);
    };

    class BinaryOperationASTNode : public ExpressionASTNode
    {
        public:
            BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs);
            ASTOperationType Operation;
            ExpressionASTNode* LeftSide;
            ExpressionASTNode* RightSide;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class UnaryOperationASTNode : public ExpressionASTNode
    {
        public:
            UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member);
            ASTOperationType Operation;
            ExpressionASTNode* Member;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class IntegerLiteralASTNode : public ExpressionASTNode
    {
        public:
            IntegerLiteralASTNode(int typeID, int value);
            int Value;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class StringLiteralASTNode : public ExpressionASTNode
    {
        public:
            StringLiteralASTNode(int typeID, string value);
            string Value;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class ArrayLiteralASTNode : public ExpressionASTNode
    {
        public:
            ArrayLiteralASTNode(int memberTypeID);
            std::vector<ExpressionASTNode*> Members;

            void Emit(DriverState& state, const char* destinationRegister);
    };

    class VariableASTNode : public ExpressionASTNode
    {
        public:
            VariableASTNode(std::string id);
            VariableASTNode(std::string id, ExpressionASTNode* index);
            std::string ID;
            ExpressionASTNode* ArrayIndex;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
            void EmitLValue(DriverState& state, const char* destinationRegister);
    };

    class ExpressionListASTNode : public ExpressionASTNode
    {
        public:
            ExpressionListASTNode();
            std::vector<ExpressionASTNode*> Members;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class FunctionCallASTNode : public ExpressionASTNode
    {
        public:
            FunctionCallASTNode(std::string id);
            FunctionCallASTNode(std::string id, ExpressionListASTNode* args);
            std::string ID;
            std::vector<ExpressionASTNode*> Arguments;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };

    class StatementListASTNode : public ASTNode
    {
        public:
            StatementListASTNode();
            bool IsGlobal;
            std::vector<ASTNode*> Members;

            void Emit(DriverState& state);
            ASTNode* Check(DriverState& state);
    };

    class SimpleDeclarationASTNode
    {
        public:
            SimpleDeclarationASTNode(int typeID, std::string id);
            int Type;
            std::string ID;
    };

    class FunctionDeclarationASTNode : public ASTNode
    {
        public:
            FunctionDeclarationASTNode(std::string id, StatementListASTNode* body);
            int Type;
            std::string ID;
            std::vector<SimpleDeclarationASTNode*>* Arguments;
            StatementListASTNode* Body;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class SingleVariableDeclarationASTNode
    {
        public:
            SingleVariableDeclarationASTNode(std::string id, int arraySize);
            SingleVariableDeclarationASTNode(std::string id, int arraySize, ExpressionASTNode* initialValue);
            std::string ID;
            ExpressionASTNode* InitialValue;
            int ArraySize;

            void Emit(DriverState& state);
    };

    class VariableDeclarationASTNode : public ASTNode
    {
        public:
            VariableDeclarationASTNode();
            int Type;
            std::vector<SingleVariableDeclarationASTNode*> Members;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class NopStatementASTNode : public ASTNode
    {
        public:
            NopStatementASTNode();

            void Emit(DriverState& state);
    };

    class ExitStatementASTNode : public ASTNode
    {
        public:
            ExitStatementASTNode();

            void Emit(DriverState& state);
    };

    class BreakStatementASTNode : public ASTNode
    {
        public:
            BreakStatementASTNode();

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ContinueStatementASTNode : public ASTNode
    {
        public:
            ContinueStatementASTNode();

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ReturnStatementASTNode : public ASTNode
    {
        public:
            ReturnStatementASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class WhileStatementASTNode : public ASTNode
    {
        public:
            WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body);
            ExpressionASTNode* Test;
            ASTNode* Body;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ForStatementASTNode : public ASTNode
    {
        public:
            ForStatementASTNode(ExpressionASTNode* initial, ExpressionASTNode* test, ExpressionASTNode* step, ASTNode* body);
            ExpressionASTNode* Initial;
            ExpressionASTNode* Test;
            ExpressionASTNode* Step;
            ASTNode* Body;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class IfStatementASTNode : public ASTNode
    {
        public:
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue);
            IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse);
            ExpressionASTNode* Test;
            ASTNode* IfTrue;
            ASTNode* IfFalse;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ReadCallASTNode : public ASTNode
    {
        public:
            ReadCallASTNode(ExpressionASTNode* variable);
            ExpressionASTNode* Variable;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class WriteCallASTNode : public ASTNode
    {
        public:
            WriteCallASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };

    class ASMASTNode : public ASTNode
    {
        public:
            ASMASTNode(string value);
            string Value;

            void Emit(DriverState& state);
    };
}}

#endif
