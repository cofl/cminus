#ifndef STATE_HPP
    #define STATE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../asm/Register.hpp"
#include "../structures/SymbolTable.hpp"

namespace Cminus
{
    using namespace std;
    using namespace Cminus::ASM;

    class State
    {
        public:
            State(ostream& outputStream, string& fileName, Structures::SymbolTable* globalTable);

        public:
            string& FileName;
            ostream& OutputStream;
            vector<Structures::SymbolTable*> SymbolStack;

            int BaseOffset;
            int GetTypeID(string&& name);
            vector<string> Types;

#pragma region Registers
        public:
            Register& GetRegister(RegisterIndex index, RegisterLength length);
            Register& AllocRegister(RegisterLength length);
            bool AllocRegister(RegisterIndex index, RegisterLength length, Register& out);
            bool RegisterStatus(Register& in);
            void SetRegisterStatus(Register& in, bool value);
            void FreeRegister(Register& in);
            void SaveRegisters(int count, ...);
            void RestoreRegisters();
        private:
            bool RegisterIsFree[14];
            Register Register8[14];
            Register Register16[14];
            Register Register32[14];
            Register Register64[14];
            vector<vector<RegisterIndex>> SavedRegisters;
#pragma endregion

#pragma region ROData
        public:
            string InternStringLiteral(string& literalValue);
            map<string, string> StringLiterals;
        private:
            int LiteralID;
#pragma endregion

#pragma region Labels
        public:
            int NextFreeLabel;
            vector<int> BreakLabels;
            vector<int> ContinueLabels;

            int GetBreakLabel();
            int GetContinueLabel();
#pragma endregion
    };
}

#endif
