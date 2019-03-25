#ifndef FunctionCallASTNode_HPP
    #define FunctionCallASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "ExpressionListASTNode.hpp"

namespace Cminus { namespace AST
{
    class FunctionCallASTNode : public ExpressionASTNode
    {
        public:
            FunctionCallASTNode(std::string id);
            FunctionCallASTNode(std::string id, ExpressionListASTNode* args);
            std::string ID;
            std::vector<ExpressionASTNode*> Arguments;

            ASTNode* Check(State& state);
            void Emit(State& state, Register& destination);
    };
}}

#endif
