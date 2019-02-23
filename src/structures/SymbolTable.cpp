#include <map>
#include <memory>
#include <string>
#include <vector>
#include "SymbolTable.hpp"

namespace Cminus { namespace Structures
{
    using namespace std;

#pragma region SymbolTable
    SymbolTable::SymbolTable()
        : _globalFrame(new SymbolTableFrame())
    {
        _frames.push_back(_globalFrame);
    }

    SymbolTable::~SymbolTable()
    {
        // TODO
    }

    void SymbolTable::EnterScope()
    {
        _frames.push_back(shared_ptr<SymbolTableFrame>(new SymbolTableFrame()));
    }

    void SymbolTable::ExitScope()
    {
        _frames.pop_back();
    }
#pragma endregion SymbolTable
#pragma region SymbolTableFrame
    SymbolTableFrame::SymbolTableFrame()
    {
        // TODO
    }

    SymbolTableFrame::~SymbolTableFrame()
    {
        // TODO
    }
#pragma endregion SymbolTableFrame
}}
