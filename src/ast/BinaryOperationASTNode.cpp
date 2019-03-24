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

    ASTNode* BinaryOperationASTNode::Check(DriverState& state)
    {
        LeftSide->Symbols = this->Symbols;
        RightSide->Symbols = this->Symbols;
        LeftSide = (ExpressionASTNode*) LeftSide->Check(state);
        RightSide = (ExpressionASTNode*) RightSide->Check(state);
        // TODO: make sure left/right types are the same
        this->Type = LeftSide->Type;
        return this;
    }

    void BinaryOperationASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        if(Operation == ASTOperationType::Assign)
        {
            //TODO: check if lvalue
            RightSide->Emit(state, destinationRegister);
            auto lsri = state.GetFreeRegister();
            auto lsrn = state.RegisterNames64[lsri];
            LeftSide->EmitLValue(state, lsrn);
            state.OutputStream << "\tmov [" << lsrn << "], " << destinationRegister << endl;
            state.ReleaseRegister(lsri);
            return;
        } else if(Operation == ASTOperationType::Divide)
        {
            auto eaxi = state.GetRegisterID("eax");
            auto edxi = state.GetRegisterID("edx");
            auto dsti = state.GetRegisterID(destinationRegister);

            // manually reserve edx if it isn't already
            auto edxf = state.FreeRegisters[edxi];
            state.FreeRegisters[edxi] = false;

            if(dsti == eaxi)
            {
                LeftSide->Emit(state, "eax");
                state.SaveRegisters(1, "rdx");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            } else if(dsti == edxi)
            {
                state.SaveRegisters(1, "rax");
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov edx, eax"  << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            } else
            {
                state.SaveRegisters(2, "rax", "rdx");
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov " << destinationRegister << ", eax"  << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            }

            // restore original reservation of edx
            state.FreeRegisters[edxi] = edxf;
            return;
        } else if(Operation == ASTOperationType::Modulo)
        {
            auto eaxi = state.GetRegisterID("eax");
            auto edxi = state.GetRegisterID("edx");
            auto dsti = state.GetRegisterID(destinationRegister);
            // manually reserve edx if it isn't already
            auto edxf = state.FreeRegisters[edxi];
            state.FreeRegisters[edxi] = false;

            if(dsti == eaxi)
            {
                LeftSide->Emit(state, "eax");
                state.SaveRegisters(1, "rdx");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov eax, edx"  << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            } else if(dsti == edxi)
            {
                state.OutputStream << "\tpush rax"  << endl;
                state.SaveRegisters(1, "rax");
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            } else
            {
                state.SaveRegisters(2, "rax", "rdx");
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov " << destinationRegister << ", edx"  << endl;
                state.RestoreRegisters();
                state.ReleaseRegister(rsri);
            }

            // restore original reservation of edx
            state.FreeRegisters[edxi] = edxf;
            return;
        }
        LeftSide->Emit(state, destinationRegister);
        auto rsri = state.GetFreeRegister();
        auto rsrn = state.RegisterNames32[rsri];
        RightSide->Emit(state, rsrn);
        auto dst8 = state.GetRegister8(destinationRegister);
        switch(Operation)
        {
            case ASTOperationType::Exponentiation:
                state.OutputStream << " ** "; // TODO
                break;
            case ASTOperationType::Multiply:
                state.OutputStream << "\timul " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::Add:
                state.OutputStream << "\tadd " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::Subtract:
                state.OutputStream << "\tsub " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::RightShift:
                state.OutputStream << "\tshr " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::LeftShift:
                state.OutputStream << "\tshl " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::BinaryXor:
                state.OutputStream << "\txor " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::BinaryOr:
                state.OutputStream << "\tor " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::LogicalOr:
                state.OutputStream << "\tor "    << destinationRegister << ", " << rsrn << endl
                                   << "\tcmp "   << destinationRegister << ", 0"        << endl
                                   << "\tsetnz " << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::BinaryAnd:
                state.OutputStream << "\tand " << destinationRegister << ", " << rsrn << endl;
                break;
            case ASTOperationType::LogicalAnd:
                state.OutputStream << "\ttest "  << destinationRegister << ", " << rsrn << endl
                                   << "\tsetnz " << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestGT:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetg "  << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestGE:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetge " << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestLT:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetl "  << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestLE:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetle " << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestNE:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetnz " << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            case ASTOperationType::TestEQ:
                state.OutputStream << "\tcmp "   << destinationRegister << ", " << rsrn << endl
                                   << "\tsetz "  << dst8                                << endl
                                   << "\tmovzx " << destinationRegister << ", " << dst8 << endl;
                break;
            default:
                state.OutputStream << "{{unrecognized binary operation}}";
        }
        state.ReleaseRegister(rsri);
    }
}}
