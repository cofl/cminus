#ifndef ASTNode_HPP
    #define ASTNode_HPP

#include "../structures/SymbolTable.hpp"
#include "../state/State.hpp"

namespace Cminus { namespace AST
{
    using namespace Cminus::Structures;

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

            virtual void Emit(State& state);
            virtual ASTNode* Check(State& state);
    };
}}

#endif
