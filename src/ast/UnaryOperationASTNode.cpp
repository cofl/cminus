#include "UnaryOperationASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    UnaryOperationASTNode::UnaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* member)
        :   ExpressionASTNode(ASTNodeType::UnaryOperation),
            Operation(operation),
            Member(member)
    {
        // nop
    }

    ASTNode* UnaryOperationASTNode::Check(State& state)
    {
        Member->Symbols = this->Symbols;
        Member->Check(state);
        if(Member->IsConstant)
        {
            // TODO
        }
        this->Type = Member->Type;
        return this;
    }

    void UnaryOperationASTNode::Emit(State& state, Register& destination)
    {
        Member->Emit(state, destination);
        auto dst8 = destination.Name(RegisterLength::_8);
        switch(Operation)
        {
            case ASTOperationType::Negation:
                state.OutputStream << "\tneg " << destination.Name() << endl;
                break;
            case ASTOperationType::BinaryNot:
                state.OutputStream << "\tnot " << destination.Name() << endl;
                break;
            case ASTOperationType::LogicalNot:
                state.OutputStream << "\tcmp "   << destination.Name() << ", 0"        << endl
                                   << "\tsetz "  << dst8                               << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8 << endl;
                break;
            default:
                state.OutputStream << "{{Unrecognized Unary Operation}}";
                break;
        }
    }
}}
