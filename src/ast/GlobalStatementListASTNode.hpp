#ifndef GlobalStatementListASTNode_HPP
    #define GlobalStatementListASTNode_HPP

#include "ASTNode.hpp"
#include "StatementListASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"

namespace Cminus { namespace AST
{
    class GlobalStatementListASTNode : public StatementListASTNode
    {
        public:
            GlobalStatementListASTNode();

            void Emit(DriverState& state);
    };
}}

#endif
