#include "Source.hpp"
#include <iostream>

namespace Cminus { namespace ASM
{
    using namespace AST;
    using std::cerr;
    using std::endl;

    Source::Source(State& state, ExpressionASTNode* expression)
    {
        switch(expression->NodeType)
        {
            case ASTNodeType::IntegerLiteral:
                Type = SourceType::Literal;
                break;
            case ASTNodeType::Variable:
                Type = SourceType::Memory;
                break;
            default:
                Type = SourceType::Register;
                Value._register = state.AllocRegister(RegisterLength::_32);
                expression->Emit(state, Value._register);
                break;
        }
    }

    ostream& operator<<(ostream& out, Source&& src)
    {

    }
}}
