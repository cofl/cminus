#ifndef LiteralASTNode_HPP
    #define LiteralASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include <string>

namespace Cminus { namespace AST
{
    class IntegerLiteralASTNode : public ExpressionASTNode
    {
        public:
            IntegerLiteralASTNode(int typeID, int value);
            int Value;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
    };

    class StringLiteralASTNode : public ExpressionASTNode
    {
        public:
            StringLiteralASTNode(int typeID, string value);
            string Value;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
    };

    class ArrayLiteralASTNode : public ExpressionASTNode
    {
        public:
            ArrayLiteralASTNode(int memberTypeID);
            std::vector<ExpressionASTNode*> Members;

            void Emit(State& state, Register& destination);
    };
}}

#endif
