#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <unordered_map>
#include "CminusDriver.hpp"
#include "CminusScanner.hpp"
#include "state/State.hpp"

using namespace std;
using namespace Cminus::AST;

namespace Cminus {
    Driver::Driver()
        : trace_parsing(false),
          trace_scanning(false)
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
        SymbolTable globalTable(nullptr);
        State state(outputStream, FileName, &globalTable);
        Root = (GlobalStatementListASTNode*) Root->Check(state);
        Root->Emit(state);
    }

    parser::symbol_type Driver::NextSymbol()
    {
        return scanner->yylex(*this);
    }
}
