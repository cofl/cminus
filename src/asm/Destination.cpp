#include "Destination.hpp"

namespace Cminus { namespace ASM
{
    Destination::Destination(State& state, ExpressionASTNode* expression)
        :   _State(state),
            Member(expression)
    {
        // TODO
    }

    Destination::Destination(State& state, Register& _register)
        :   _State(state),
            Member(nullptr)
    {
        // TODO
    }

    void Destination::Prepare()
    {
        // TODO
    }

    void Destination::Cleanup()
    {
        // TODO
    }

    ostream& operator<<(ostream& out, Destination& dest)
    {
        // TODO
        return out;
    }
}}
