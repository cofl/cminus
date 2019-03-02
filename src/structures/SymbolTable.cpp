#include <map>
#include <memory>
#include <string>
#include <vector>
#include "SymbolTable.hpp"

namespace Cminus { namespace Structures
{
    using namespace std;

    VariableState::VariableState(SymbolTable* introducedTable)
        : IntroducedTable(introducedTable)
    {
        // nop
    }

    FunctionState::FunctionState(SymbolTable* introducedTable)
        : IntroducedTable(introducedTable)
    {
        // nop
    }

    SymbolTable::SymbolTable(SymbolTable* parent)
        : Parent(parent)
    {
        // nop
    }

    VariableState* SymbolTable::FindVariable(string& name)
    {
        auto iterator = Variables.find(name);
        if(iterator != Variables.end())
            return iterator->second;
        if(nullptr != Parent)
            return Parent->FindVariable(name);
        return nullptr;
    }

    VariableState* SymbolTable::AddVariable(string& name)
    {
        auto state = new VariableState(this);
        Variables[name] = state;
        return state;
    }

    FunctionState* SymbolTable::FindFunction(string& name)
    {
        auto iterator = Functions.find(name);
        if(iterator != Functions.end())
            return iterator->second;
        if(nullptr != Parent)
            return Parent->FindFunction(name);
        return nullptr;
    }

    FunctionState* SymbolTable::AddFunction(string& name)
    {
        auto state = new FunctionState(this);
        Functions[name] = state;
        return state;
    }
}}
