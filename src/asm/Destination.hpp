#ifndef Destination_HPP
    #define Destination_HPP

#include <iostream>
#include "../state/State.hpp"
#include "../ast/ExpressionASTNode.hpp"
#include "Location.hpp"

namespace Cminus { namespace ASM
{
    using namespace AST;

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
            LocationType Type;
            Register _Register;
        // TODO
    };
}}

#endif
