#include "ASM.hpp"
#include "../state/State.hpp"
#include <iostream>
#include <string>

namespace Cminus { namespace ASM
{
    using std::string;
    using std::endl;

    void Label(State& state, int labelID)
    {
        state.OutputStream << labelID << ':' << endl;
    }

    void Label(State& state, string& label)
    {
        state.OutputStream << label << ':' << endl;
    }

    void Label(State& state, const string& label)
    {
        state.OutputStream << label << ':' << endl;
    }

    void Section(State& state, SectionType sectionType)
    {
        switch (sectionType)
        {
            case SectionType::Text:
                state.OutputStream << "\t.text" << endl;
                break;
            case SectionType::ROData:
                state.OutputStream << "\t.section .rodata" << endl;
                break;
        }
    }

    void JumpForward(State& state, int labelID)
    {
        state.OutputStream << "\tjmp " << labelID << 'f' << endl;
    }

    void JumpBackward(State& state, int labelID)
    {
        state.OutputStream << "\tjmp " << labelID << 'b' << endl;
    }

    void CmpAndJump(State& state, const string& jmpOperation, int labelID, char direction, Register& dest)
    {
        state.OutputStream << "\tcmp " << dest << ", 0"                            << endl
                           << "\t" << jmpOperation << ' ' << labelID << direction << endl;
    }

    void Call(State& state, const string& label)
    {
        state.OutputStream << "\tcall " << label << endl;
    }

    void Variable(State& state, VariableASTNode* variable)
    {
        Variable(state.OutputStream, variable);
    }

    void Variable(ostream& stream, VariableASTNode* variable)
    {
        if(nullptr != variable->ArrayIndex)
            throw "ASM::Variable only supports variables without array indices.";
        auto data = variable->Symbols->FindVariable(variable->ID);
        if(data->IsGlobal)
        {
            stream << data->GlobalLocation << "[rip]";
        } else
        {
            stream << data->StackOffset << "[rbp]";
        }
    }

    void EncodeString(State& state, string& value)
    {
        state.OutputStream << '"';
        int length = value.length();
        for(int i = 0; i < length; i += 1)
        {
            auto chr = value[i];
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
        state.OutputStream << "\"";
    }

    void Verbatim(State& state, string& line)
    {
        state.OutputStream << line;
    }

    void Verbatim(ostream& out, string& line)
    {
        out << line;
    }

    void Verbatim(State& state, const string& line)
    {
        state.OutputStream << line;
    }

    void Verbatim(ostream& out, const string& line)
    {
        out << line;
    }

    void Verbatim(ostream& out, int constant)
    {
        out << constant;
    }

    void Verbatim(ostream& out, Register& reg)
    {
        out << reg;
    }

    void VerbatimLine(State& state, string& line)
    {
        state.OutputStream << line << endl;
    }

    void VerbatimLine(State& state, const string& line)
    {
        state.OutputStream << line << endl;
    }

    void EndLine(State& state)
    {
        state.OutputStream << endl;
    }

    void Store(State& state, int stackOffset, Register& src)
    {
        state.OutputStream << "\tmov " << stackOffset << "[rbp], " << src << endl;
    }

    void Store(State& state, int stackOffset, const string& base, Register& src)
    {
        state.OutputStream << "\tmov " << stackOffset << "[" << base << "], " << src << endl;
    }

    void Store(State& state, Register& dest, Register& src)
    {
        state.OutputStream << "\tmov [" << dest.Name(RegisterLength::_64) << "], " << src << endl;
    }

    void LoadGlobalAddress(State& state, Register& dest, const string& labelName)
    {
        state.OutputStream << "\tlea " << dest.Name(RegisterLength::_64) << ", " << labelName << "[rip]" << endl;
    }

    void Load(State& state, Register& dest, string& globalName)
    {
        state.OutputStream << "\tmov " << dest << ", " << globalName << "[rip]" << endl;
    }

    void Load(State& state, Register& dest, int stackOffset)
    {
        state.OutputStream << "\tmov " << dest << ", " << stackOffset << "[rbp]" << endl;
    }

    void Zero(State& state, Register& dest)
    {
        state.OutputStream << "\tmov " << dest << ", 0" << endl;
    }

    void Move(State& state, Register& dest, Register& src)
    {
        state.OutputStream << "\tmov " << dest << ", " << src << endl;
    }

    void Move(State& state, Register& dest, int value)
    {
        state.OutputStream << "\tmov " << dest << ", " << value << endl;
    }

    void Operation(State& state, const string& operation, Register& dest)
    {
        state.OutputStream << "\t" << operation << ' ' << dest << endl;
    }

    void Operation(State& state, const string& operation, Register& dest, Source& src)
    {
        src.Prepare();
        state.OutputStream << "\t" << operation << ' ' << dest << ", " << src << endl;
        src.Cleanup();
    }

    void CmpAndSet(State& state, const string& setOperation, Register& dest)
    {
        auto byteName = dest.Name(RegisterLength::_8);
        state.OutputStream << "\tcmp " << dest << ", 0"               << endl
                           << "\t" << setOperation << ' ' << byteName << endl
                           << "\tmovzx " << dest << ", "  << byteName << endl;
    }

    void CmpAndSet(State& state, const string& setOperation, Register& dest, Source& src)
    {
        src.Prepare();
        auto byteName = dest.Name(RegisterLength::_8);
        state.OutputStream << "\tcmp " << dest << ", " << src         << endl
                           << "\t" << setOperation << ' ' << byteName << endl
                           << "\tmovzx " << dest << ", "  << byteName << endl;
        src.Cleanup();
    }

    void TestAndSet(State& state, const string& setOperation, Register& dest, Source& src)
    {
        src.Prepare();
        auto byteName = dest.Name(RegisterLength::_8);
        state.OutputStream << "\ttest " << dest << ", " << src        << endl
                           << "\t" << setOperation << ' ' << byteName << endl
                           << "\tmovzx " << dest << ", "  << byteName << endl;
        src.Cleanup();
    }

    void IncreaseStack(State& state, int size)
    {
        state.GetStackOffset() += size;
        state.OutputStream << "\tsub rsp, " << size << endl;
    }

    void DecreaseStack(State& state, int size)
    {
        state.GetStackOffset() -= size;
        state.OutputStream << "\tadd rsp, " << size << endl;
    }

    void FunctionHeader(State& state, string& functionName)
    {
        state.StackOffset.push_back(0);
        state.OutputStream << "\t.global\t" << functionName         << endl
                    << "\t.type\t" << functionName << ", @function" << endl
                    << functionName << ":"                          << endl
                    << "\tpush rbp"                                 << endl
                    << "\tmov rbp, rsp"                             << endl;
    }

    void FunctionFooter(State& state, string& functionName)
    {
        state.StackOffset.pop_back();
        state.OutputStream << "\tmov eax, 0" << endl
                           << "\tleave"      << endl
                           << "\tret"        << endl
                           << "\t.size\t" << functionName << ", .-" << functionName << endl;
    }

    void Return(State& state)
    {
        state.OutputStream << "\tleave" << endl
                           << "\tret"   << endl;
    }
}}
