#ifndef GlobalStatementListASTNode_HPP
    #define GlobalStatementListASTNode_HPP

#include "ASTNode.hpp"
#include "StatementListASTNode.hpp"

namespace Cminus { namespace AST
{
    class GlobalStatementListASTNode : public StatementListASTNode
    {
        public:
            GlobalStatementListASTNode();

            void Emit(State& state);
    };
}}

#endif
