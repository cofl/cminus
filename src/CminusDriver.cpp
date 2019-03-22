#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <cstdarg>
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
        Root = (StatementListASTNode*) Root->Check(state);
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
          BreakLabels(),
          ContinueLabels(),
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
          },
          TemporaryLabelIndex(1)
    {
        SymbolStack.push_back(globalTable);
        for(int i = 0; i < 14; i += 1)
        {
            RegisterNameToId[RegisterNames32[i]] = i;
            RegisterNameToId[RegisterNames64[i]] = i;
            RegisterNameToId[RegisterNames8[i]] = i;
        }
    }

    void DriverState::SaveRegisters(int count, ...)
    {
        auto saved = new vector<const char*>();
        va_list args;
        va_start(args, count);
        for(;count > 0; count -= 1)
        {
            auto reg = va_arg(args, const char*);
            auto regi = RegisterNameToId[reg];
            if(!FreeRegisters[regi])
            {
                saved->push_back(reg);
                OutputStream << "\tpush " << reg << endl;
                FreeRegisters[regi] = true;
            }
        }
        SavedRegisters.push_back(saved);
    }

    void DriverState::RestoreRegisters()
    {
        auto saved = SavedRegisters.back();
        SavedRegisters.pop_back();
        for(int i = saved->size() - 1; i >= 0; i -= 1)
        {
            auto member = saved->at(i);
            auto regi = RegisterNameToId[member];
            OutputStream << "\tpop " << member << endl;
            FreeRegisters[regi] = false;
        }
        delete saved;
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

    int DriverState::GetBreakLabel()
    {
        return BreakLabels.size() > 0 ? BreakLabels.back() : -1;
    }

    int DriverState::GetContinueLabel()
    {
        return ContinueLabels.size() > 0 ? ContinueLabels.back() : -1;
    }
}
