#ifndef StatementListASTNode_HPP
    #define StatementListASTNode_HPP

#include "ASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class StatementListASTNode : public ASTNode
    {
        public:
            StatementListASTNode();
            bool IsGlobal;
            std::vector<ASTNode*> Members;

            void Emit(DriverState& state);
            ASTNode* Check(DriverState& state);
    };
}}

#endif
