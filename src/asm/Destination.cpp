#include "Destination.hpp"
#include <iostream>
#include "../ast/VariableASTNode.hpp"

namespace Cminus { namespace ASM
{
    Destination::Destination(State& state, ExpressionASTNode* expression)
        :   _State(state),
            Member(expression)
    {
        switch(expression->NodeType)
        {
            case ASTNodeType::IntegerLiteral:
                Type = LocationType::Literal;
                break;
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
                _Register = _State.AllocRegister(RegisterLength::_32);
                Member->Emit(_State, _Register);
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
        // TODO
        return out;
    }
}}
