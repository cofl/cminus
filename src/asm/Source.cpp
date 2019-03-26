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
                Type = SourceType::Literal;
                break;
            case ASTNodeType::Variable:
                if(nullptr == ((VariableASTNode*) Member)->ArrayIndex)
                {
                    Type = SourceType::Memory;
                    break;
                }
            default:
                Type = SourceType::Register;
                break;
        }
    }

    void Source::Prepare()
    {
        switch(Type)
        {
            case SourceType::Register:
                _Register = _State.AllocRegister(RegisterLength::_32);
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
            case SourceType::Register:
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
            case SourceType::Literal:
                ASM::Verbatim(out, ((IntegerLiteralASTNode*) src.Member)->Value);
                break;
            case SourceType::Memory:
                ASM::Variable(out, (VariableASTNode*) src.Member);
                break;
            case SourceType::Register:
                ASM::Verbatim(out, src._Register);
                break;
            default:
                break;
        }
        return out;
    }
}}
