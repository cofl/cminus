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

    ASTNode* UnaryOperationASTNode::Check(DriverState& state)
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

    void UnaryOperationASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        Member->Emit(state, destinationRegister);
        auto dst8 = state.GetRegister8(destinationRegister);
        switch(Operation)
        {
            case ASTOperationType::Negation:
                state.OutputStream << "\tneg " << destinationRegister << endl;
                break;
            case ASTOperationType::BinaryNot:
                state.OutputStream << "\tnot " << destinationRegister << endl;
                break;
            case ASTOperationType::LogicalNot:
                state.OutputStream << "\tcmp "   << destinationRegister << ", 0"        << endl
                                   << "\tsetz "  << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            default:
                state.OutputStream << "{{Unrecognized Unary Operation}}";
                break;
        }
    }
}}
