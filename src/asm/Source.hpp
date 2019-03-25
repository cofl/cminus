#ifndef Source_HPP
    #define Source_HPP
#include "../state/State.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/ExpressionASTNode.hpp"
#include "../ast/VariableASTNode.hpp"
#include <iostream>

namespace Cminus { namespace ASM
{
    using namespace AST;

    enum class SourceType
    {
        Register = 0,
        Literal = 1,
        Memory = 2
    };

    class Source
    {
        public:
            Source(State& state, ExpressionASTNode* expression);

            friend ostream& operator<<(ostream& out, Source&& src);

        private:
            SourceType Type;
            union {
                Register _register;
                int _literal;
                string _location;
            } Value;
        // TODO
    };
}}

#endif
