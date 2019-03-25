#ifndef ASMASTNode_HPP
    #define ASMASTNode_HPP

#include "ASTNode.hpp"
#include <string>

namespace Cminus { namespace AST
{
    class ASMASTNode : public ASTNode
    {
        public:
            ASMASTNode(string value);
            string Value;

            void Emit(State& state);
    };
}}

#endif
