#ifndef IOStatement_HPP
    #define IOStatement_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"

namespace Cminus { namespace AST
{
    class ReadCallASTNode : public ASTNode
    {
        public:
            ReadCallASTNode(ExpressionASTNode* variable);
            ExpressionASTNode* Variable;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };

    class WriteCallASTNode : public ASTNode
    {
        public:
            WriteCallASTNode(ExpressionASTNode* value);
            ExpressionASTNode* Value;

            ASTNode* Check(State& state);
            void Emit(State& state);
    };
}}

#endif
