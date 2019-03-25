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

    void JumpForward(State& state, int labelID)
    {
        state.OutputStream << "\tjmp " << labelID << 'f' << endl;
    }

    void JumpBackward(State& state, int labelID)
    {
        state.OutputStream << "\tjmp " << labelID << 'b' << endl;
    }

    void JumpBackwardIfFalse(State& state, Register& reg, int labelID)
    {
        state.OutputStream << "\tcmp " << reg.Name() << ", 0" << endl
                           << "\tjnz " << labelID << 'b'      << endl;
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

    void Verbatim(State& state, const string& line)
    {
        state.OutputStream << line;
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
}}
