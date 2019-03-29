#ifndef Source_HPP
    #define Source_HPP
#include "../state/State.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/ExpressionASTNode.hpp"
#include "../ast/VariableASTNode.hpp"
#include "Location.hpp"
#include <iostream>

namespace Cminus { namespace ASM
{
    using namespace AST;

    class Source
    {
        public:
            Source(State& state, ExpressionASTNode* expression);
            Source(State& state, Register& _register);
            Source(State& state, ExpressionASTNode* expression, Register& _register);

            void Prepare();
            void Cleanup();
            friend ostream& operator<<(ostream& out, Source& src);
        private:
            State& _State;
            ExpressionASTNode* Member;
            LocationType Type;
            Register _Register;
        // TODO
    };
}}

#endif
