#include "Destination.hpp"
#include <iostream>
#include "../asm/ASM.hpp"
#include "../ast/VariableASTNode.hpp"

namespace Cminus { namespace ASM
{
    using namespace AST;
    using std::cerr;
    using std::endl;

    Destination::Destination(State& state, ExpressionASTNode* expression)
        :   _State(state),
            Member(expression)
    {
        switch(expression->NodeType)
        {
            case ASTNodeType::Variable:
                if(nullptr == ((VariableASTNode*) Member)->ArrayIndex)
                {
                    Type = LocationType::Memory;
                    break;
                }
            default:
                Type = LocationType::Register;
                break;
        }
    }

    Destination::Destination(State& state, Register& _register)
        :   _State(state),
            Member(nullptr),
            _Register(_register),
            Type(LocationType::BoundRegister)
    {
        // nop
    }

    void Destination::Prepare()
    {
        switch(Type)
        {
            case LocationType::Register:
                _Register = _State.AllocRegister(RegisterLength::_64);
                Member->EmitLValue(_State, _Register);
                break;
            default:
                break;
        }
    }

    void Destination::Cleanup()
    {
        switch(Type)
        {
            case LocationType::Register:
                _State.FreeRegister(_Register);
                break;
            default:
                break;
        }
    }

    ostream& operator<<(ostream& out, Destination& dest)
    {
        switch (dest.Type)
        {
            case LocationType::Memory:
                ASM::Variable(out, (VariableASTNode*) dest.Member);
                break;
            case LocationType::BoundRegister:
            case LocationType::Register:
                ASM::Variable(out, dest._Register);
                break;
            default:
                break;
        }
        return out;
    }
}}
