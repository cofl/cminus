#ifndef Destination_HPP
    #define Destination_HPP

#include <iostream>
#include "../state/State.hpp"
#include "../ast/ExpressionASTNode.hpp"

namespace Cminus { namespace ASM
{
    using namespace AST;

    enum class DestinationType
    {
        Register = 0,
        Literal = 1,
        Memory = 2
    };

    class Destination
    {
        public:
            Destination(State& state, ExpressionASTNode* expression);
            Destination(State& state, Register& _register);

            void Prepare();
            void Cleanup();
            friend ostream& operator<<(ostream& out, Destination& dest);
        private:
            State& _State;
            ExpressionASTNode* Member;
            DestinationType Type;
            Register _Register;
        // TODO
    };
}}

#endif
