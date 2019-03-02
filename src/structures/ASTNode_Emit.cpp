#include "../DriverState.hpp"
#include "ASTNode.hpp"
#include <iostream>
#include <typeinfo>
#include <string>
#include <sys/utsname.h>

namespace Cminus { namespace Structures
{
    using std::endl;
    using std::is_same;

    void ASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "{{Unknown Node}}" << endl;
    }

    void ExpressionASTNode::Emit(DriverState& state)
    {
        auto index = state.GetFreeRegister();
        Emit(state, state.RegisterNames32[index]);
        state.ReleaseRegister(index);
    }

    void ExpressionASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }

    void ExpressionASTNode::EmitLValue(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "{{Unknown expresion}}" << endl;
    }

    void StatementListASTNode::Emit(DriverState& state)
    {
        if(IsGlobal)
        {
            state.OutputStream << "\t.file \"" << *(state.FileName) << "\"" << endl
                               << "\t.intel_syntax noprefix /* I have to be difficult because I like to read */" << endl
                               << "\t.text"             << endl;
            int uninitializedGlobalCount = 0;
            for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++)
            {
                auto data = it->second;
                data->IsGlobal = true;
                if(nullptr == data->InitialValue)
                {
                    data->Location = std::string("_gp[rip+").append(to_string(uninitializedGlobalCount * 4)).append("]");
                    uninitializedGlobalCount += 1;
                    continue;
                }
                // TODO: emit constant code
            }
            if(uninitializedGlobalCount > 0)
                state.OutputStream << "\t.comm\t_gp," << (uninitializedGlobalCount * 4) << ",4" << endl;
            state.OutputStream << "\t.section .rodata"  << endl
                               << ".int_wformat:"       << endl
                               << "\t.string \"%d\\n\"" << endl
                               << ".int_rformat:"       << endl
                               << "\t.string \"%d\""    << endl;
            for(int i = 0; i < state.StringConstants.size(); i += 1)
            {
                state.OutputStream << ".string_constant" << i << ":" << endl
                                   << "\t.string \"";
                auto string = state.StringConstants[i];
                auto length = string.length();
                for(int i = 0; i < length; i += 1)
                {
                    auto chr = string[i];
                    switch(chr)
                    {
                        case '\b': state.OutputStream << "\\b"; break;
                        case '\f': state.OutputStream << "\\f"; break;
                        case '\n': state.OutputStream << "\\n"; break;
                        case '\r': state.OutputStream << "\\r"; break;
                        case '\t': state.OutputStream << "\\t"; break;
                        case '\\': state.OutputStream << "\\\\"; break;
                        case '\"': state.OutputStream << "\\\""; break;
                        default:
                            if(chr >= ' ' && chr <= '~' && chr != '\\' && chr != '\"')
                            {
                                state.OutputStream << chr;
                            } else
                            {
                                char upper = (chr & 0xF0) >> 4;
                                char lower = chr & 0x0F;
                                state.OutputStream << "\\x"
                                                   << (upper > 9 ? upper + 'A' : upper + '0')
                                                   << (lower > 9 ? lower + 'A' : lower + '0');
                            }
                            break;
                    }
                }
                state.OutputStream << "\"" << endl;
            }
            state.OutputStream << "\t.text" << endl
                               << "\t.global\tmain" << endl
                               ;
        } else
        {
            if(!Symbols->Variables.empty())
            {
                // reserve space on the stack
                state.OutputStream << "\tsub rsp, " << Symbols->GetAlignedSize() << endl;
                int i = 1;
                for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++, i += 1)
                {
                    auto data = it->second;
                    data->Location = to_string(-(i * 4)).append("[rbp]");
                    if(nullptr != data->InitialValue)
                    {
                        // TODO: emit initializer code
                    }
                }
            }
        }
        for(auto const& member: Members)
            member->Emit(state);
        if(IsGlobal)
        {
            utsname uts;
            uname(&uts);
            state.OutputStream << "\t.ident \"cllacour-CMC: ("
                               << uts.sysname << "~" << uts.release
                               << ") 0.0.2\"" << endl;
            // GCC thinks this is a good idea, and so does the internet
            state.OutputStream << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
        }
    }

    void IfStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "if (";
        Test->Emit(state);
        state.OutputStream << ")" << endl;
        IfTrue->Emit(state);
        if(IfFalse != nullptr)
        {
            state.OutputStream << "else" << endl;
            IfFalse->Emit(state);
        }
    }

    void WhileStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "while (";
        Test->Emit(state);
        state.OutputStream << ")" << endl;
        Body->Emit(state);
    }

    void FunctionDeclarationASTNode::Emit(DriverState& state)
    {

        state.OutputStream << "\t.type\tmain, @function" << endl
                           << ID << ":" << endl
                           << "\tpush rbp" << endl
                           << "\tmov rbp, rsp" << endl;
        Body->Emit(state);
        state.OutputStream << "\tleave" << endl
                           << "\tret" << endl
                           << "\t.size\t" << ID << ", .-" << ID << endl;
    }

    void VariableDeclarationASTNode::Emit(DriverState& state)
    {
        // TODO?
    }

    void ArrayLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        // TODO
    }

    void WriteCallASTNode::Emit(DriverState& state)
    {
        if(Value->Type == state.GetTypeID("string"))
        {
            // emit string write
            auto rdii = state.GetRegisterID("rdi");
            state.FreeRegisters[rdii] = false; // manually reserve register
            Value->Emit(state, "rdi");
            state.OutputStream << "\tcall puts@PLT" << endl; // mildly more efficient than printf
            state.FreeRegisters[rdii] = true; // manually free register
        } else
        {
            // emit value generation, then value write
            auto esii = state.GetRegisterID("edi");
            state.FreeRegisters[esii] = false; // manually reserve register
            Value->Emit(state, "esi");
            state.OutputStream << "\tlea rdi, .int_wformat[rip]" << endl
                               << "\tmov eax, 0"                 << endl
                               << "\tcall printf@PLT"            << endl;
            state.FreeRegisters[esii] = true; // manually free register
        }
    }

    void ReadCallASTNode::Emit(DriverState& state)
    {
        auto rsii = state.GetRegisterID("rsi");
        state.FreeRegisters[rsii] = false; // manually reserve register
        Variable->EmitLValue(state, "rsi");
        state.OutputStream << "\tlea rdi, .int_rformat[rip]" << endl
                           << "\tmov eax, 0"                 << endl
                           << "\tcall scanf@PLT"             << endl;
        state.FreeRegisters[rsii] = true; // manually free register
    }

    void ExpressionListASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        for(auto const& member: Members)
            member->Emit(state, destinationRegister);
    }

    void FunctionCallASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        // TODO
        state.OutputStream << ID << "(";
        auto separator = "";
        for(auto const& member: Arguments)
        {
            state.OutputStream << separator;
            separator = ", ";
            member->Emit(state);
        }
        state.OutputStream << ")";
    }

    void VariableASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "\tmov " << destinationRegister << ", " << (Symbols->FindVariable(ID)->Location) << endl;
    }

    void VariableASTNode::EmitLValue(DriverState& state, const char* destinationRegister)
    {
        auto data = Symbols->FindVariable(ID);
        state.OutputStream << "\tlea " << destinationRegister << ", " << data->Location << endl;
    }

    void ExitStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "{{exit;}}" << endl;
    }

    void BreakStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "{{break;}}" << endl;
    }

    void ReturnStatementASTNode::Emit(DriverState& state)
    {
        if(nullptr != Value)
            Value->Emit(state, "eax");
        state.OutputStream << "\tleave" << endl
                           << "\tret"   << endl;
    }

    void BinaryOperationASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        if(Operation == ASTOperationType::Assign)
        {
            //TODO: check if lvalue
            auto lsri = state.GetFreeRegister();
            auto lsrn = state.RegisterNames64[lsri];
            LeftSide->EmitLValue(state, lsrn);
            RightSide->Emit(state, destinationRegister);
            state.OutputStream << "\tmov [" << lsrn << "], " << destinationRegister << endl;
            state.ReleaseRegister(lsri);
            return;
        } else if(Operation == ASTOperationType::ArrayAccess)
        {
            // TODO: make sure our destination can take 64bits
            auto lsri = state.GetFreeRegister();
            auto lsrn = state.RegisterNames64[lsri];
            LeftSide->Emit(state, lsrn);
            RightSide->Emit(state, destinationRegister);
            state.OutputStream << "\tmov " << destinationRegister << ", [" << lsrn << "+" << destinationRegister << "]" << endl;
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
                state.OutputStream << "\tpush rdx"   << endl
                                   << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tpop rdx"       << endl;
                state.ReleaseRegister(rsri);
            } else if(dsti == edxi)
            {
                state.OutputStream << "\tpush rax"  << endl;
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov edx, eax"  << endl
                                   << "\tpop rax"       << endl;
                state.ReleaseRegister(rsri);
            } else
            {
                state.OutputStream << "\tpush rax"  << endl
                                   << "\tpush rdx"  << endl;
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov " << destinationRegister << ", eax"  << endl
                                   << "\tpop rdx"       << endl
                                   << "\tpop rax"       << endl;
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
                state.OutputStream << "\tpush rdx"   << endl
                                   << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov eax, edx"  << endl
                                   << "\tpop rdx"       << endl;
                state.ReleaseRegister(rsri);
            } else if(dsti == edxi)
            {
                state.OutputStream << "\tpush rax"  << endl;
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tpop rax"       << endl;
                state.ReleaseRegister(rsri);
            } else
            {
                state.OutputStream << "\tpush rax"  << endl
                                   << "\tpush rdx"  << endl;
                LeftSide->Emit(state, "eax");
                state.OutputStream << "\tmov edx, 0" << endl;
                auto rsri = state.GetFreeRegister();
                auto rsrn = state.RegisterNames32[rsri];
                RightSide->Emit(state, rsrn);
                state.OutputStream << "\tidiv " << rsrn << endl
                                   << "\tmov " << destinationRegister << ", edx"  << endl
                                   << "\tpop rdx"       << endl
                                   << "\tpop rax"       << endl;
                state.ReleaseRegister(rsri);
            }

            // restore original reservation of edx
            state.FreeRegisters[edxi] = edxf;
            return;
        }
        auto rsri = state.GetFreeRegister();
        auto rsrn = state.RegisterNames32[rsri];
        LeftSide->Emit(state, destinationRegister);
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

    void IntegerLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "\tmov " << destinationRegister << ", " << Value << endl;
    }

    void StringLiteralASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        state.OutputStream << "\tlea " << destinationRegister << ", " << Value << "[rip]" << endl;
    }
}}
