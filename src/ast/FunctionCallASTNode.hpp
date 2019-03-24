#ifndef FunctionCallASTNode_HPP
    #define FunctionCallASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "ExpressionListASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
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
}}

#endif
