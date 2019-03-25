#include <iostream>
#include <string>
#include <cstdarg>
#include "State.hpp"
#include "../asm/Register.hpp"

namespace Cminus
{
    using namespace std;
    using namespace Cminus::ASM;

    State::State(ostream& outputStream, string& fileName, Structures::SymbolTable* globalTable)
        :   OutputStream(outputStream),
            FileName(fileName),
            LiteralID(0),
            StringLiterals(),
            NextFreeLabel(0),
            StackOffset(),
            BreakLabels(),
            ContinueLabels(),
            SavedRegisters(),
            SymbolStack(),
            Types(),
            RegisterIsFree {
                true, true, true, true, true, true, true,
                true, true, true, true, true, true, true
            }
    {
        SymbolStack.push_back(globalTable);
        Types.push_back("int");
        Types.push_back("string");
    }

    const Register& State::AllocRegister(RegisterLength length)
    {
        for(int i = 0; i < 14; i += 1)
        {
            if(RegisterIsFree[i])
            {
                RegisterIsFree[i] = false;
                return Register::Get((RegisterIndex) i, length);
            }
        }
        throw "No register is available!";
    }

    bool State::AllocRegister(RegisterIndex index, RegisterLength length, Register& out)
    {
        if (!RegisterIsFree[(int) index])
            return false;
        RegisterIsFree[(int) index] = false;
        out = Register::Get(index, length);
        return true;
    }

    bool State::RegisterStatus(Register& in)
    {
        return RegisterIsFree[(int) in.Length];
    }

    void State::SetRegisterStatus(Register& in, bool value)
    {
        RegisterIsFree[(int) in.Length] = value;
    }

    void State::FreeRegister(Register& in)
    {
        RegisterIsFree[(int) in.Index] = true;
    }

    void State::SaveRegisters(int count, ...)
    {
        vector<RegisterIndex> saved;
        va_list args;
        va_start(args, count);
        for(;count > 0; count -= 1)
        {
            auto regi = va_arg(args, RegisterIndex);
            auto reg = Register::Get(regi, RegisterLength::_64);
            if(!RegisterIsFree[(int) regi])
            {
                saved.push_back(regi);
                OutputStream << "\tpush " << reg.Name() << endl;
                RegisterIsFree[(int) regi] = true;
            }
        }
        SavedRegisters.push_back(saved);
    }

    void State::RestoreRegisters()
    {
        auto saved = SavedRegisters.back();
        SavedRegisters.pop_back();
        for(int i = saved.size() - 1; i >= 0; i -= 1)
        {
            auto reg = Register::Get(saved[i], RegisterLength::_64);
            OutputStream << "\tpop " << reg.Name() << endl;
            RegisterIsFree[(int) saved[i]] = false;
        }
    }

    string State::InternStringLiteral(string& literalValue)
    {
        auto it = StringLiterals.find(literalValue);
        if(it != StringLiterals.end())
            return it->first;

        auto name = std::string(".string_constant").append(to_string(LiteralID++));
        StringLiterals[name] = literalValue;
        return name;
    }

    int State::GetBreakLabel()
    {
        return BreakLabels.size() > 0 ? BreakLabels.back() : -1;
    }

    int State::GetContinueLabel()
    {
        return ContinueLabels.size() > 0 ? ContinueLabels.back() : -1;
    }

    int State::GetTypeID(string&& typeName)
    {
        for(int i = 0; i < Types.size(); i += 1)
            if(Types[i] == typeName)
                return i;
        throw string("Unrecognized type name \"").append(typeName).append("\".");
    }

    int& State::GetStackOffset()
    {
        return StackOffset.back();
    }
}
