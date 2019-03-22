#include "../DriverState.hpp"
#include "ASTNode.hpp"
#include <iostream>
#include <typeinfo>
#include <string>
#include <sys/utsname.h>
#include <regex>

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
            for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++)
            {
                auto name = it->first;
                auto data = it->second;
                data->IsGlobal = true;
                if(nullptr == data->InitialValue)
                {
                    data->GlobalLocation = name;
                    state.OutputStream << "\t.comm\t" << name << "," << (4 * data->ArraySize) << ",4" << endl;
                    continue;
                }
                // TODO: emit constant code
            }
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
            state.OutputStream << "\t.text" << endl;
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
                    data->IsGlobal = false;
                    data->StackOffset = i * data->ArraySize * -4 - state.BaseOffset;
                    if(nullptr != data->InitialValue)
                    {
                        // TODO: emit initializer code
                    }
                }
                state.BaseOffset += Symbols->GetAlignedSize();
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
                               << ") 0.0.3\"" << endl;
            // GCC thinks this is a good idea, and so does the internet
            state.OutputStream << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
        } else if(!Symbols->Variables.empty())
        {
            state.BaseOffset -= Symbols->GetAlignedSize();
        }
    }

    void IfStatementASTNode::Emit(DriverState& state)
    {
        int elseLabel = state.TemporaryLabelIndex;
        state.TemporaryLabelIndex += 1;
        Test->Emit(state, "eax");
        state.OutputStream << "\tcmp eax, 0"              << endl
                           << "\tje " << elseLabel << 'f' << endl;
        IfTrue->Emit(state);
        if(IfFalse != nullptr)
        {
            int afterLabel = state.TemporaryLabelIndex;
            state.TemporaryLabelIndex += 1;
            state.OutputStream << "\tjmp " << afterLabel << 'f' << endl
                               << elseLabel << ':'              << endl;
            IfFalse->Emit(state);
            state.OutputStream << afterLabel << ':' << endl;
            state.TemporaryLabelIndex -= 2;
        } else
        {
            state.OutputStream << elseLabel << ':' << endl;
            state.TemporaryLabelIndex -= 1;
        }
    }

    void WhileStatementASTNode::Emit(DriverState& state)
    {
        int beginLabel = state.TemporaryLabelIndex;
        int testLabel = state.TemporaryLabelIndex + 1;
        int afterLabel = state.TemporaryLabelIndex + 2;
        state.TemporaryLabelIndex += 3;
        state.ContinueLabels.push_back(testLabel);
        state.BreakLabels.push_back(afterLabel);
        state.OutputStream << "\tjmp " << testLabel << 'f'  << endl
                           << beginLabel << ':'             << endl;
        Body->Emit(state);
        state.OutputStream << testLabel << ':'              << endl;
        Test->Emit(state, "eax");
        state.OutputStream << "\tcmp eax, 0"                << endl
                           << "\tjnz " << beginLabel << 'b' << endl
                           << afterLabel << ':'             << endl;
        state.TemporaryLabelIndex -= 2;
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }

    void ForStatementASTNode::Emit(DriverState& state)
    {
        int beginLabel = state.TemporaryLabelIndex;
        int stepLabel = state.TemporaryLabelIndex + 1;
        int afterLabel = state.TemporaryLabelIndex + 2;
        state.TemporaryLabelIndex += 3;
        state.ContinueLabels.push_back(stepLabel);
        state.BreakLabels.push_back(afterLabel);
        if(nullptr != Initial)
            Initial->Emit(state);
        Test->Emit(state, "eax");
        state.OutputStream << "\tcmp eax, 0"               << endl
                           << "\tje " << afterLabel << 'f' << endl
                           << beginLabel << ':'            << endl;
        Body->Emit(state);
        state.OutputStream << stepLabel << ':' << endl;
        if(nullptr != Step)
            Step->Emit(state);
        Test->Emit(state, "eax");
        state.OutputStream << "\tcmp eax, 0"                << endl
                           << "\tjnz " << beginLabel << 'b' << endl
                           << afterLabel << ':'             << endl;
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }

    void BreakStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tjmp " << state.GetBreakLabel() << 'f' << endl;
    }

    void ContinueStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tjmp " << state.GetContinueLabel() << 'f' << endl;
    }

    void FunctionDeclarationASTNode::Emit(DriverState& state)
    {
        int prevBaseOffset = state.BaseOffset;
        state.BaseOffset = 0;
        state.OutputStream << "\t.global\t" << ID                << endl
                           << "\t.type\t" << ID << ", @function" << endl
                           << ID << ":"                          << endl
                           << "\tpush rbp"                       << endl
                           << "\tmov rbp, rsp"                   << endl;
        if(nullptr != Arguments)
        {
            int argc = Arguments->size();
            if(argc > 0)
            {
                // make space to put arguments on the stack.
                int t = 4 * (argc > 6 ? 6 : argc);
                t += 16 - (t % 16);
                state.OutputStream << "\tsub rsp," << t << endl;
                state.BaseOffset = +t;
            }
            for(int i = 0; i < Arguments->size(); i += 1)
            {
                auto arg = Arguments->at(i);
                auto data = Symbols->Variables[arg->ID];
                data->IsGlobal = false;
                switch(i)
                {
                    case 0:
                        data->StackOffset = -4;
                        state.OutputStream << "\tmov -4[rbp],edi" << endl;
                        break;
                    case 1:
                        data->StackOffset = -8;
                        state.OutputStream << "\tmov -8[rbp],esi" << endl;
                        break;
                    case 2:
                        data->StackOffset = -12;
                        state.OutputStream << "\tmov -12[rbp],edx" << endl;
                        break;
                    case 3:
                        data->StackOffset = -16;
                        state.OutputStream << "\tmov -16[rbp],ecx" << endl;
                        break;
                    case 4:
                        data->StackOffset = -20;
                        state.OutputStream << "\tmov -20[rbp],r8d" << endl;
                        break;
                    case 5:
                        data->StackOffset = -24;
                        state.OutputStream << "\tmov -24[rbp],r9d" << endl;
                        break;
                    default:
                        data->StackOffset = 8 + i * 4;
                }
            }
        }
        Body->Emit(state);
        state.OutputStream << "\tmov eax, 0" << endl
                           << "\tleave"      << endl
                           << "\tret"        << endl
                           << "\t.size\t" << ID << ", .-" << ID << endl;
        state.BaseOffset = prevBaseOffset;
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
        bool usingEax = state.GetRegisterID(destinationRegister) == state.GetRegisterID("eax");
        state.SaveRegisters(6, "rdi", "rsi", "rdx", "rcx", "r8", "r9");
        int argc = Arguments.size();
        if(argc > 0)
            Arguments[0]->Emit(state, "edi");
        if (argc > 1)
            Arguments[1]->Emit(state, "esi");
        if (argc > 2)
            Arguments[2]->Emit(state, "edx");
        if (argc > 3)
            Arguments[3]->Emit(state, "ecx");
        if (argc > 4)
            Arguments[4]->Emit(state, "r8d");
        if (argc > 5)
            Arguments[5]->Emit(state, "r9d");
        if (argc > 6)
        {
            int t = 4 * (argc - 6);
            t += 16 - (t % 16);
            state.OutputStream << "\tsub rsp," << t << endl;
            auto tri = state.GetFreeRegister();
            auto trn = state.RegisterNames32[tri];
            for(int i = 6, j = 0; i < argc; i += 1, j += 1)
            {
                Arguments[i]->Emit(state, trn);
                state.OutputStream << "\tmov " << j << "[rsp]," << trn << endl;
            }
            state.ReleaseRegister(tri);
        }
        state.OutputStream << "\tmov eax, 0"  << endl
                           << "\tcall " << ID << endl;
        if(!usingEax)
            state.OutputStream << "\tmov " << destinationRegister << ", eax" << endl;
        state.RestoreRegisters();
    }

    void VariableASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
                if (savedAx)
                    state.OutputStream << "\tpush rax" << endl;
                ArrayIndex->Emit(state, "eax");
                auto tri = state.GetFreeRegister();
                auto trn64 = state.RegisterNames64[tri];
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << trn64 << ", 0[0+rax*4]"              << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]" << endl
                                   << "\tmov " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
                state.ReleaseRegister(tri);
                if (savedAx)
                    state.OutputStream << "\tpop rax" << endl;
            } else
            {
                state.OutputStream << "\tmov " << destinationRegister << ", " << data->GlobalLocation << "[rip]" << endl;
            }
        } else if (nullptr != ArrayIndex)
        {
            auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
            if (savedAx)
                state.OutputStream << "\tpush rax" << endl;
            ArrayIndex->Emit(state, "eax");
            auto tri = state.GetFreeRegister();
            auto trn64 = state.RegisterNames64[tri];
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << trn64 << ", 0[0+rax*4]"           << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]" << endl
                               << "\tmov " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
            state.ReleaseRegister(tri);
            if (savedAx)
                state.OutputStream << "\tpop rax" << endl;
        } else
        {
            state.OutputStream << "\tmov " << destinationRegister << ", " << data->StackOffset << "[rbp]" << endl;
        }
    }

    void VariableASTNode::EmitLValue(DriverState& state, const char* destinationRegister)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
                if (savedAx)
                    state.OutputStream << "\tpush rax" << endl;
                ArrayIndex->Emit(state, "eax");
                auto tri = state.GetFreeRegister();
                auto trn64 = state.RegisterNames64[tri];
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << trn64 << ", 0[0+rax*4]"              << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]" << endl
                                   << "\tlea " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
                state.ReleaseRegister(tri);
                if (savedAx)
                    state.OutputStream << "\tpop rax" << endl;
            } else
            {
                state.OutputStream << "\tlea " << destinationRegister << ", " << data->GlobalLocation << "[rip]" << endl;
            }
        } else if (nullptr != ArrayIndex)
        {
            auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
            if (savedAx)
                state.OutputStream << "\tpush rax" << endl;
            ArrayIndex->Emit(state, "eax");
            auto tri = state.GetFreeRegister();
            auto trn64 = state.RegisterNames64[tri];
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << trn64 << ", 0[0+rax*4]"           << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]" << endl
                               << "\tlea " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
            state.ReleaseRegister(tri);
            if (savedAx)
                state.OutputStream << "\tpop rax" << endl;
        } else
        {
            state.OutputStream << "\tlea " << destinationRegister << ", " << data->StackOffset << "[rbp]" << endl;
        }
    }

    void NopStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tnop" << endl;
    }

    void ExitStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "{{exit;}}" << endl;
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

    void ASMASTNode::Emit(DriverState& state)
    {
        smatch rgxMatch;
        regex varExpr("\\$([a-zA-Z_][a-zA-Z0-9_]*)");
        while(regex_search(Value, rgxMatch, varExpr))
        {
            auto varID = rgxMatch[1].str();
            auto data = Symbols->FindVariable(varID);
            state.OutputStream << rgxMatch.prefix().str();
            if(data->IsGlobal)
                state.OutputStream << data->GlobalLocation << "[rip]";
            else
                state.OutputStream << data->StackOffset << "[rbp]";
            Value = rgxMatch.suffix().str();
        }
        state.OutputStream << Value << endl;
    }
}}
