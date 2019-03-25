#ifndef ASM_HPP
    #define ASM_HPP

#include "../state/State.hpp"
#include "Register.hpp"
#include <string>

namespace Cminus { namespace ASM
{
    using std::string;

    void Label(State& state, int labelID);
    void Label(State& state, string& label);
    void Label(State& state, const string& label);

    void JumpForward(State& state, int labelID);
    void JumpBackward(State& state, int labelID);

    void JumpBackwardIfFalse(State& state, Register& registerName, int labelID);

    void EncodeString(State& state, string& value);

    void Verbatim(State& state, string& line);
    void Verbatim(State& state, const string& line);
    void VerbatimLine(State& state, string& line);
    void VerbatimLine(State& state, const string& line);
    void EndLine(State& state);
}}

#endif
