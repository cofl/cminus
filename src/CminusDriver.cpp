#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <unordered_map>
#include "CminusDriver.hpp"
#include "DriverState.hpp"
#include "CminusScanner.hpp"

using namespace std;

namespace Cminus {
    Driver::Driver()
        : trace_parsing(false),
          trace_scanning(false),
          GlobalSymbolTable(nullptr)
    {
        Types.insert(Types.begin(), "int");
        Types.push_back("string");
    }

    int Driver::Parse(const string& fileName, istream* inputStream, ostream* outputStream)
    {
        FileName = fileName;
        Location.initialize (&FileName);

        auto& in = *inputStream;
        scanner = new Scanner(in);
        scanner->set_debug(trace_scanning);

        Cminus::parser parser(*this, *outputStream);
        parser.set_debug_level (trace_parsing);
        return parser.parse();
    }

    int Driver::GetTypeID(string&& typeName)
    {
        for(int i = 0; i < Types.size(); i += 1)
            if(Types[i] == typeName)
                return i;
        throw string("Unrecognized type name \"").append(typeName).append("\".");
    }

    void Driver::Process(ostream& outputStream)
    {
        DriverState state(outputStream, &GlobalSymbolTable, &FileName, &Types);
        Root->Symbols = &GlobalSymbolTable;
        Root->IsGlobal = true;
        Root->Check(state);
        Root->Emit(state);
    }

    parser::symbol_type Driver::NextSymbol()
    {
        return scanner->yylex(*this);
    }

    DriverState::DriverState(
        ostream& outputStream,
        SymbolTable* globalTable,
        string* fileName,
        vector<string>* types)
        : OutputStream(outputStream),
          SymbolStack(),
          FileName(fileName),
          Types(types),
          RegisterNames8 {
              "al", "bl", "cl", "dl",
              "sil", "dil", "r8b", "r9b",
              "r10b", "r11b", "r12b", "r13b",
              "r14b", "r15b"
          },
          RegisterNames32 {
              "eax", "ebx", "ecx", "edx",
              "esi", "edi", "r8d", "r9d",
              "r10d", "r11d", "r12d", "r13d",
              "r14d", "r15d"
          },
          RegisterNames64 {
              "rax", "rbx", "rcx", "rdx",
              "rsi", "rdi", "r8", "r9",
              "r10", "r11", "r12", "r13",
              "r14", "r15"
          },
          FreeRegisters {
              false, true, true, true, // eax is reserved
              true, true, true, true,
              true, true, true, true,
              true, true
          }
    {
        SymbolStack.push_back(globalTable);
        for(int i = 0; i < 14; i += 1)
        {
            RegisterNameToId[RegisterNames32[i]] = i;
            RegisterNameToId[RegisterNames64[i]] = i;
            RegisterNameToId[RegisterNames8[i]] = i;
        }
    }

    int DriverState::AddStringConstant(string& value)
    {
        StringConstants.push_back(value);
        return StringConstants.size() - 1;
    }

    int DriverState::GetTypeID(string&& typeName)
    {
        for(int i = 0; i < Types->size(); i += 1)
            if(Types->at(i) == typeName)
                return i;
        throw string("Unrecognized type name \"").append(typeName).append("\".");
    }

    int DriverState::GetFreeRegister()
    {
        for(int i = 1; i < 14; i += 1) // eax is reserved, so we skip over it
            if(FreeRegisters[i])
            {
                FreeRegisters[i] = false;
                return i;
            }
        return -1;
    }

    void DriverState::ReleaseRegister(int registerIndex)
    {
        FreeRegisters[registerIndex] = true;
    }

    int DriverState::GetRegisterID(string&& registerName)
    {
        auto it = RegisterNameToId.find(registerName);
        if(it != RegisterNameToId.end())
            return it->second;
        return -1;
    }

    const char* DriverState::GetRegister8(string&& registerName)
    {
        auto it = RegisterNameToId.find(registerName);
        if(it != RegisterNameToId.end())
            return RegisterNames8[it->second];
        return nullptr;
    }
}
