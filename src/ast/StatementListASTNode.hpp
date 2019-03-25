#ifndef StatementListASTNode_HPP
    #define StatementListASTNode_HPP

#include "ASTNode.hpp"

namespace Cminus { namespace AST
{
    class StatementListASTNode : public ASTNode
    {
        public:
            StatementListASTNode();
            std::vector<ASTNode*> Members;

            void Emit(State& state);
            ASTNode* Check(State& state);
    };
}}

#endif
