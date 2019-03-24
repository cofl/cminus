#ifndef ExpressionListASTNode_HPP
    #define ExpressionListASTNode_HPP

#include "ASTNode.hpp"
#include "ExpressionASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class ExpressionListASTNode : public ExpressionASTNode
    {
        public:
            ExpressionListASTNode();
            std::vector<ExpressionASTNode*> Members;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state, const char* destinationRegister);
    };
}}

#endif
