#include "Register.hpp"

#include <iostream>
#include <map>

namespace Cminus { namespace ASM
{
    using std::string;
    using std::map;
    using std::ostream;

    static const string RegisterNames[17][5] = {
        { "al", "ax", "eax", "rax", "ah" },
        { "bl", "bx", "ebx", "rbx", "bh" },
        { "cl", "cx", "ecx", "rcx", "ch" },
        { "dl", "dx", "edx", "rdx", "dh" },
        { "sil", "si", "esi", "rsi", "%" },
        { "dil", "di", "edi", "rdi", "%" },
        { "r8b", "r8w", "r8d", "r8", "%" },
        { "r9b", "r9w", "r9d", "r9", "%" },
        { "r10b", "r10w", "r10d", "r10", "%"},
        { "r11b", "r11w", "r11d", "r11", "%"},
        { "r12b", "r12w", "r12d", "r12", "%"},
        { "r13b", "r13w", "r13d", "r13", "%"},
        { "r14b", "r14w", "r14d", "r14", "%"},
        { "r15b", "r15w", "r15d", "r15", "%"},
        { "%", "%", "%", "rbp", "%" },
        { "%", "%", "%", "rsp", "%" },
        { "%", "%", "%", "rip", "%" }
    };

    static const Register Registers8[17] = {
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
        Register(RegisterIndex::R15B, RegisterLength::_8),
        Register(RegisterIndex::RBP, RegisterLength::_64),
        Register(RegisterIndex::RSP, RegisterLength::_64),
        Register(RegisterIndex::RIP, RegisterLength::_64)
    };

    static const Register Registers16[17] = {
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
        Register(RegisterIndex::R15W, RegisterLength::_16),
        Register(RegisterIndex::RBP, RegisterLength::_64),
        Register(RegisterIndex::RSP, RegisterLength::_64),
        Register(RegisterIndex::RIP, RegisterLength::_64)
    };
    static const Register Registers32[17] {
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
        Register(RegisterIndex::R15D, RegisterLength::_32),
        Register(RegisterIndex::RBP, RegisterLength::_64),
        Register(RegisterIndex::RSP, RegisterLength::_64),
        Register(RegisterIndex::RIP, RegisterLength::_64)
    };
    static const Register Registers64[17] = {
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
        Register(RegisterIndex::R15, RegisterLength::_64),
        Register(RegisterIndex::RBP, RegisterLength::_64),
        Register(RegisterIndex::RSP, RegisterLength::_64),
        Register(RegisterIndex::RIP, RegisterLength::_64)
    };

    static const map<RegisterLength,int> RegisterLengthToNameIndex ({
        {RegisterLength::_8, 1},
        {RegisterLength::_16, 2},
        {RegisterLength::_32, 3},
        {RegisterLength::_64, 4}
    });

    Register::Register()
    {
        // nop
    }

    Register::Register(RegisterIndex index, RegisterLength length)
        :   Index(index),
            Length(length)
    {
        // nop
    }

    const string& Register::Name()
    {
        return Name(Length);
    }

    const string& Register::Name(RegisterLength length)
    {
        switch (length)
        {
            case RegisterLength::_8:
                return RegisterNames[(int)Index][0];
            case RegisterLength::_16:
                return RegisterNames[(int)Index][1];
            case RegisterLength::_32:
                return RegisterNames[(int)Index][2];
            case RegisterLength::_64:
                return RegisterNames[(int)Index][3];
        }
    }

    ostream& operator<<(ostream& stream, Register& reg)
    {
        return stream << reg.Name();
    }

    const Register& Register::Get(RegisterIndex index)
    {
        return Registers32[(int) index];
    }

    const Register& Register::Get(RegisterIndex index, RegisterLength length)
    {
        switch (length)
        {
            case RegisterLength::_8:
                return Registers8[(int) index];
            case RegisterLength::_16:
                return Registers16[(int) index];
            case RegisterLength::_32:
                return Registers32[(int) index];
            case RegisterLength::_64:
                return Registers64[(int) index];
        }
    }
}}
