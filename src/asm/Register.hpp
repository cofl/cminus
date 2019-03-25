#ifndef Register_HPP
    #define Register_HPP

#include <cstdint>
#include <string>
#include <iostream>

namespace Cminus { namespace ASM
{
    using std::string;
    using std::ostream;

    enum class RegisterIndex : uint8_t
    {
        AL = 0,     AX = AL,        EAX = AL,       RAX = AL,
        BL = 1,     BX = BL,        EBX = BL,       RBX = BL,
        CL = 2,     CX = CL,        ECX = CL,       RCX = CL,
        DL = 3,     DX = DL,        EDX = DL,       RDX = DL,
        SIL = 4,    SI = SIL,       ESI = SIL,      RSI = SIL,
        DIL = 5,    DI = DIL,       EDI = DIL,      RDI = DIL,
        R8B = 6,    R8W = R8B,      R8D  = R8B,     R8 = R8B,
        R9B = 7,    R9W = R9B,      R9D  = R9B,     R9 = R9B,
        R10B = 8,   R10W = R10B,    R10D = R10B,    R10 = R10B,
        R11B = 9,   R11W = R11B,    R11D = R11B,    R11 = R11B,
        R12B = 10,  R12W = R12B,    R12D = R12B,    R12 = R12B,
        R13B = 11,  R13W = R13B,    R13D = R13B,    R13 = R13B,
        R14B = 12,  R14W = R14B,    R14D = R14B,    R14 = R14B,
        R15B = 13,  R15W = R15B,    R15D = R15B,    R15 = R15B,
        RBP = 14,
        RSP = 15,
        RIP = 16
    };

    enum class RegisterLength : uint8_t
    {
        _8  = 1,
        _16 = 2,
        _32 = 4,
        _64 = 8
    };

    class Register
    {
        public:
            Register();
            Register(RegisterIndex index, RegisterLength length);

            RegisterIndex Index;
            RegisterLength Length;

            const string& Name();
            const string& Name(RegisterLength otherlength);
            friend ostream& operator<<(ostream& stream, Register& reg);
        private:
    };
}}

#endif
