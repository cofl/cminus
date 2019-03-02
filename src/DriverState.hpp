#ifndef DriverState_HPP
    #define DriverState_HPP
    #include <iostream>
    #include <vector>
    #include <map>
    #include <string>
    #include "structures/SymbolTable.hpp"

namespace Cminus
{
    using namespace std;

    class DriverState
    {
        public:
            DriverState(ostream& outputStream, Structures::SymbolTable* globalTable, string* fileName, vector<string>* types);
            string* FileName;
            ostream& OutputStream;
            vector<Structures::SymbolTable*> SymbolStack;
            vector<string> StringConstants;
            int AddStringConstant(string& value);

            vector<string>* Types;
            int GetTypeID(string&& typeName);
            int GetFreeRegister();
            void ReleaseRegister(int registerIndex);
            const char* RegisterNames32[14];
            const char* RegisterNames64[14];
            int GetRegisterID(string&& registerName);
            const char* GetRegister8(string&& registerName);
            bool FreeRegisters[14];
            void SaveRegisters(const char* registerName...);
            void RestoreRegisters();
        private:
            const char* RegisterNames8[14];
            vector<vector<char*>> SavedRegisters;
            map<string,int> RegisterNameToId;
    };
}

#endif
