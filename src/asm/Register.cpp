#include "Register.hpp"

#include <map>

namespace Cminus { namespace ASM
{
    using std::string;
    using std::map;

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
}}
