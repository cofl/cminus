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
            BaseOffset(0),
            BreakLabels(),
            ContinueLabels(),
            SavedRegisters(),
            SymbolStack(),
            Types(),
            RegisterIsFree {
                true, true, true, true, true, true, true,
                true, true, true, true, true, true, true
            },
            Register8{
                Register(RegisterIndex::AL, RegisterLength::_8),
                Register(RegisterIndex::BL, RegisterLength::_8),
                Register(RegisterIndex::CL, RegisterLength::_8),
                Register(RegisterIndex::DL, RegisterLength::_8),
                Register(RegisterIndex::SIL, RegisterLength::_8),
                Register(RegisterIndex::DIL, RegisterLength::_8),
                Register(RegisterIndex::R8B, RegisterLength::_8),
                Register(RegisterIndex::R9B, RegisterLength::_8),
                Register(RegisterIndex::R10B, RegisterLength::_8),
                Register(RegisterIndex::R11B, RegisterLength::_8),
                Register(RegisterIndex::R12B, RegisterLength::_8),
                Register(RegisterIndex::R13B, RegisterLength::_8),
                Register(RegisterIndex::R14B, RegisterLength::_8),
                Register(RegisterIndex::R15B, RegisterLength::_8)
            },
            Register16{
                Register(RegisterIndex::AX, RegisterLength::_16),
                Register(RegisterIndex::BX, RegisterLength::_16),
                Register(RegisterIndex::CX, RegisterLength::_16),
                Register(RegisterIndex::DX, RegisterLength::_16),
                Register(RegisterIndex::SI, RegisterLength::_16),
                Register(RegisterIndex::DI, RegisterLength::_16),
                Register(RegisterIndex::R8W, RegisterLength::_16),
                Register(RegisterIndex::R9W, RegisterLength::_16),
                Register(RegisterIndex::R10W, RegisterLength::_16),
                Register(RegisterIndex::R11W, RegisterLength::_16),
                Register(RegisterIndex::R12W, RegisterLength::_16),
                Register(RegisterIndex::R13W, RegisterLength::_16),
                Register(RegisterIndex::R14W, RegisterLength::_16),
                Register(RegisterIndex::R15W, RegisterLength::_16)
            },
            Register32 {
                Register(RegisterIndex::EAX, RegisterLength::_32),
                Register(RegisterIndex::EBX, RegisterLength::_32),
                Register(RegisterIndex::ECX, RegisterLength::_32),
                Register(RegisterIndex::EDX, RegisterLength::_32),
                Register(RegisterIndex::ESI, RegisterLength::_32),
                Register(RegisterIndex::EDI, RegisterLength::_32),
                Register(RegisterIndex::R8D, RegisterLength::_32),
                Register(RegisterIndex::R9D, RegisterLength::_32),
                Register(RegisterIndex::R10D, RegisterLength::_32),
                Register(RegisterIndex::R11D, RegisterLength::_32),
                Register(RegisterIndex::R12D, RegisterLength::_32),
                Register(RegisterIndex::R13D, RegisterLength::_32),
                Register(RegisterIndex::R14D, RegisterLength::_32),
                Register(RegisterIndex::R15D, RegisterLength::_32)
            },
            Register64 {
                Register(RegisterIndex::RAX, RegisterLength::_64),
                Register(RegisterIndex::RBX, RegisterLength::_64),
                Register(RegisterIndex::RCX, RegisterLength::_64),
                Register(RegisterIndex::RDX, RegisterLength::_64),
                Register(RegisterIndex::RSI, RegisterLength::_64),
                Register(RegisterIndex::RDI, RegisterLength::_64),
                Register(RegisterIndex::R8, RegisterLength::_64),
                Register(RegisterIndex::R9, RegisterLength::_64),
                Register(RegisterIndex::R10, RegisterLength::_64),
                Register(RegisterIndex::R11, RegisterLength::_64),
                Register(RegisterIndex::R12, RegisterLength::_64),
                Register(RegisterIndex::R13, RegisterLength::_64),
                Register(RegisterIndex::R14, RegisterLength::_64),
                Register(RegisterIndex::R15, RegisterLength::_64)
            }
    {
        RegisterIsFree[0] = false;
        SymbolStack.push_back(globalTable);
        Types.push_back("int");
        Types.push_back("string");
    }

    Register& State::AllocRegister(RegisterLength length)
    {
        for(int i = 0; i < 14; i += 1)
        {
            if(RegisterIsFree[i])
            {
                RegisterIsFree[i] = false;
                switch (length)
                {
                    case RegisterLength::_8:
                        return Register8[(int) i];
                    case RegisterLength::_16:
                        return Register16[(int) i];
                    case RegisterLength::_32:
                        return Register32[(int) i];
                    case RegisterLength::_64:
                        return Register64[(int) i];
                }
            }
        }
        throw "No register is available!";
    }

    bool State::AllocRegister(RegisterIndex index, RegisterLength length, Register& out)
    {
        if (!RegisterIsFree[(int) index])
            return false;
        RegisterIsFree[(int) index] = false;
        switch (length)
        {
            case RegisterLength::_8:
                out = Register8[(int) index];
                return true;
            case RegisterLength::_16:
                out = Register16[(int) index];
                return true;
            case RegisterLength::_32:
                out = Register32[(int) index];
                return true;
            case RegisterLength::_64:
                out = Register64[(int) index];
                return true;
        }
    }

    Register& State::GetRegister(RegisterIndex index, RegisterLength length)
    {
        switch (length)
        {
            case RegisterLength::_8:
                return Register8[(int) index];
            case RegisterLength::_16:
                return Register16[(int) index];
            case RegisterLength::_32:
                return Register32[(int) index];
            case RegisterLength::_64:
                return Register64[(int) index];
        }
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
            auto reg = GetRegister(regi, RegisterLength::_64);
            if(RegisterIsFree[(int) regi])
            {
                saved.push_back(regi);
                OutputStream << "\tpush " << reg.Name() << endl;
                RegisterIsFree[(int) regi] = false;
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
            auto reg = GetRegister(saved[i], RegisterLength::_64);
            OutputStream << "\tpop " << reg.Name() << endl;
            RegisterIsFree[(int) saved[i]] = true;
        }
    }

    string State::InternStringLiteral(string& literalValue)
    {
        auto it = StringLiterals.find(literalValue);
        if(it != StringLiterals.end())
            return it->first;

        auto name = ".string_constant" + LiteralID++;
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
}
