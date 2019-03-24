#ifndef IOStatement_HPP
    #define IOStatement_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
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
}}

#endif
