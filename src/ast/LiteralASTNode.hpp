#ifndef LiteralASTNode_HPP
    #define LiteralASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"
#include <string>

namespace Cminus { namespace AST
{
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
}}

#endif
