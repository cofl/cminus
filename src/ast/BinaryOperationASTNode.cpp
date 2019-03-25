#include "BinaryOperationASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    BinaryOperationASTNode::BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs)
        :   ExpressionASTNode(ASTNodeType::BinaryOperation),
            Operation(operation),
            LeftSide(lhs),
            RightSide(rhs)
    {
        // TODO: type
    }

    ASTNode* BinaryOperationASTNode::Check(State& state)
    {
        LeftSide->Symbols = this->Symbols;
        RightSide->Symbols = this->Symbols;
        LeftSide = (ExpressionASTNode*) LeftSide->Check(state);
        RightSide = (ExpressionASTNode*) RightSide->Check(state);
        // TODO: make sure left/right types are the same
        this->Type = LeftSide->Type;
        return this;
    }

    void BinaryOperationASTNode::Emit(State& state, Register& destination)
    {
        if(Operation == ASTOperationType::Assign)
        {
            //TODO: check if lvalue
            RightSide->Emit(state, destination);
            auto lsr = state.AllocRegister(RegisterLength::_64);
            LeftSide->EmitLValue(state, lsr);
            state.OutputStream << "\tmov [" << lsr.Name() << "], " << destination.Name() << endl;
            state.FreeRegister(lsr);
            return;
        } else if(Operation == ASTOperationType::Divide)
        {
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            auto edx = state.GetRegister(RegisterIndex::EDX, RegisterLength::_32);

            // manually reserve edx if it isn't already
            auto edxs = state.RegisterStatus(edx);
            state.SetRegisterStatus(edx, false);

            if(destination.Index == RegisterIndex::EAX)
            {
                LeftSide->Emit(state, destination);
                state.SaveRegisters(1, RegisterIndex::RDX);
            } else if(destination.Index == RegisterIndex::EDX)
            {
                state.SaveRegisters(1, RegisterIndex::RAX);
                LeftSide->Emit(state, eax);
            } else
            {
                state.SaveRegisters(2, RegisterIndex::RAX, RegisterIndex::RDX);
                LeftSide->Emit(state, eax);
            }

            state.OutputStream << "\tmov edx, 0" << endl;
            auto rsr = state.AllocRegister(RegisterLength::_32);
            RightSide->Emit(state, rsr);
            state.OutputStream << "\tidiv " << rsr.Name() << endl;

            if (destination.Index != RegisterIndex::EAX)
            {
                state.OutputStream << "\tmov " << destination.Name() << ", eax" << endl;
            }

            // cleanup, restore edx state
            state.FreeRegister(rsr);
            state.RestoreRegisters();
            state.SetRegisterStatus(edx, edxs);
            return;
        } else if(Operation == ASTOperationType::Modulo)
        {
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            auto edx = state.GetRegister(RegisterIndex::EDX, RegisterLength::_32);

            // manually reserve edx if it isn't already
            auto edxs = state.RegisterStatus(edx);
            state.SetRegisterStatus(edx, false);

            if(destination.Index == RegisterIndex::EAX)
            {
                LeftSide->Emit(state, destination);
                state.SaveRegisters(1, RegisterIndex::RDX);
            } else if(destination.Index == RegisterIndex::EDX)
            {
                state.SaveRegisters(1, RegisterIndex::RAX);
                LeftSide->Emit(state, eax);
            } else
            {
                state.SaveRegisters(2, RegisterIndex::RAX, RegisterIndex::RDX);
                LeftSide->Emit(state, eax);
            }

            state.OutputStream << "\tmov edx, 0" << endl;
            auto rsr = state.AllocRegister(RegisterLength::_32);
            RightSide->Emit(state, rsr);
            state.OutputStream << "\tidiv " << rsr.Name() << endl;

            if (destination.Index != RegisterIndex::EDX)
            {
                state.OutputStream << "\tmov " << destination.Name() << ", edx" << endl;
            }

            // cleanup, restore edx state
            state.FreeRegister(rsr);
            state.RestoreRegisters();
            state.SetRegisterStatus(edx, edxs);
            return;
        }

        LeftSide->Emit(state, destination);

        auto rsr = state.AllocRegister(RegisterLength::_32);
        auto dst8 = destination.Name(RegisterLength::_8);
        RightSide->Emit(state, rsr);
        switch(Operation)
        {
            case ASTOperationType::Exponentiation:
                state.OutputStream << " ** "; // TODO
                break;
            case ASTOperationType::Multiply:
                state.OutputStream << "\timul " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::Add:
                state.OutputStream << "\tadd " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::Subtract:
                state.OutputStream << "\tsub " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::RightShift:
                state.OutputStream << "\tshr " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::LeftShift:
                state.OutputStream << "\tshl " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::BinaryXor:
                state.OutputStream << "\txor " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::BinaryOr:
                state.OutputStream << "\tor " << destination.Name() << ", " << rsr.Name() << endl;
                break;
            case ASTOperationType::LogicalOr:
                state.OutputStream << "\tor "    << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tcmp "   << destination.Name() << ", 0"              << endl
                                   << "\tsetnz " << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::BinaryAnd:
                state.OutputStream << "\tand " << destination.Name() << ", " << rsr.Name()   << endl;
                break;
            case ASTOperationType::LogicalAnd:
                state.OutputStream << "\ttest "  << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetnz " << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestGT:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetg "  << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestGE:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetge " << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestLT:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetl "  << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestLE:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetle " << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestNE:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetnz " << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            case ASTOperationType::TestEQ:
                state.OutputStream << "\tcmp "   << destination.Name() << ", " << rsr.Name() << endl
                                   << "\tsetz "  << dst8                                     << endl
                                   << "\tmovzx " << destination.Name() << ", " << dst8       << endl;
                break;
            default:
                state.OutputStream << "{{unrecognized binary operation}}";
        }
        state.FreeRegister(rsr);
    }
}}
