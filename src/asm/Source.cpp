#include "Source.hpp"
#include <iostream>
#include "ASM.hpp"
#include "../ast/LiteralASTNode.hpp"

namespace Cminus { namespace ASM
{
    using namespace AST;
    using std::cerr;
    using std::endl;

    Source::Source(State& state, ExpressionASTNode* expression)
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

    Source::Source(State& state, Register& _register)
        :   _State(state),
            Member(nullptr),
            _Register(_register),
            Type(LocationType::BoundRegister)
    {
        // nop
    }

    Source::Source(State& state, ExpressionASTNode* expression, Register& _register)
        :   _State(state),
            Member(expression),
            _Register(_register),
            Type(LocationType::BoundRegister)
    {
        // nop
    }

    void Source::Prepare()
    {
        switch(Type)
        {
            case LocationType::Register:
                _Register = _State.AllocRegister(RegisterLength::_32);
            case LocationType::BoundRegister:
                if (nullptr != Member)
                    Member->Emit(_State, _Register);
                break;
            default:
                break;
        }
    }

    void Source::Cleanup()
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

    ostream& operator<<(ostream& out, Source& src)
    {
        switch(src.Type)
        {
            case LocationType::Literal:
                ASM::Verbatim(out, ((IntegerLiteralASTNode*) src.Member)->Value);
                break;
            case LocationType::Memory:
                ASM::Variable(out, (VariableASTNode*) src.Member);
                break;
            case LocationType::BoundRegister:
            case LocationType::Register:
                ASM::Verbatim(out, src._Register);
                break;
            default:
                break;
        }
        return out;
    }
}}
