#ifndef SymbolTable_HPP
    #define SymbolTable_HPP

#include <vector>
#include <map>
#include <string>

namespace Cminus { namespace Structures
{
    using namespace std;

    class SymbolTable;

    class VariableState
    {
        public:
            VariableState(SymbolTable* introducedTable);
            SymbolTable* IntroducedTable;
            int Type;
            int ArraySize;
            int* InitialValue;
            bool IsGlobal;
            string GlobalLocation;
            int StackOffset;
    };

    class FunctionArgument
    {
        public:
            int TypeID;
            string Name;
    };

    class FunctionState
    {
        public:
            FunctionState(SymbolTable* introducedTable);
            SymbolTable* IntroducedTable;
            int ReturnType;
            vector<FunctionArgument> Arguments;
    };

    class SymbolTable
    {
        public:
            SymbolTable(SymbolTable* parent);
            SymbolTable* Parent;

            map<string, VariableState*> Variables;
            VariableState* FindVariable(string& name);
            VariableState* AddVariable(string& name);

            map<string, FunctionState*> Functions;
            FunctionState* FindFunction(string& name);
            FunctionState* AddFunction(string& name);

            int GetAlignedSize();
    };
}}

#endif
