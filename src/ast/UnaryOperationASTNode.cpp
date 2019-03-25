#include "UnaryOperationASTNode.hpp"
#include "../asm/ASM.hpp"
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
        switch(Operation)
        {
            case ASTOperationType::Negation:    ASM::Operation(state, "neg", destination); break;
            case ASTOperationType::BinaryNot:   ASM::Operation(state, "not", destination); break;
            case ASTOperationType::LogicalNot:  ASM::CmpAndSet(state, "setz", destination); break;
            default:
                state.OutputStream << "{{Unrecognized Unary Operation}}";
                break;
        }
    }
}}
