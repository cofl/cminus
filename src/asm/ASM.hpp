#ifndef ASM_HPP
    #define ASM_HPP

#include "../state/State.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/VariableASTNode.hpp"
#include "Register.hpp"
#include "Source.hpp"
#include <string>

namespace Cminus { namespace ASM
{
    using std::string;

    enum class SectionType
    {
        Text,
        ROData,
    };

    void Label(State& state, int labelID);
    void Label(State& state, string& label);
    void Label(State& state, const string& label);
    void Section(State& state, SectionType sectionType);

    void JumpForward(State& state, int labelID);
    void JumpBackward(State& state, int labelID);
    void CmpAndJump(State& state, const string& jmpOperation, int labelID, char direction, Register& dest);

    void Call(State& state, const string& label);

    void Variable(State& state, VariableASTNode* variable);
    void Variable(ostream& stream, VariableASTNode* variable);
    void EncodeString(State& state, string& value);

    void Verbatim(State& state, string& line);
    void Verbatim(ostream& out, string& line);
    void Verbatim(State& state, const string& line);
    void Verbatim(ostream& out, const string& line);
    void Verbatim(ostream& out, int constant);
    void Verbatim(ostream& out, Register& reg);
    void VerbatimLine(State& state, string& line);
    void VerbatimLine(State& state, const string& line);
    void EndLine(State& state);

    void Store(State& state, int stackOffset, Register& src);
    void Store(State& state, int stackOffset, const string& base, Register& src);
    void Store(State& state, Register& dest, Register& src);
    void Load(State& state, Register& dest, string& globalName);
    void Load(State& state, Register& dest, int stackOffset);
    void LoadGlobalAddress(State& state, Register& dest, const string& labelName);
    void Zero(State& state, Register& dest);
    void Move(State& state, Register& dest, Register& src);
    void Move(State& state, Register& dest, int value);
    void Operation(State& state, const string& operation, Register& dest);
    void Operation(State& state, const string& operation, Register& dest, Source& src);
    void CmpAndSet(State& state, const string& setOperation, Register& dest);
    void CmpAndSet(State& state, const string& setOperation, Register& dest, Source& src);
    void TestAndSet(State& state, const string& setOperation, Register& dest, Source& src);

    void IncreaseStack(State& state, int size);
    void DecreaseStack(State& state, int size);
    void FunctionHeader(State& state, string& functionName);
    void FunctionFooter(State& state, string& functionName);
    void Return(State& state);
}}

#endif
