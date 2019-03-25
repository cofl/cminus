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
            ~Source();

            friend ostream& operator<<(ostream& out, Source& src);

        private:
            State& _State;
            ExpressionASTNode* Member;
            SourceType Type;
            Register _Register;
        // TODO
    };
}}

#endif
