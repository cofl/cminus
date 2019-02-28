#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <unordered_map>
#include "CminusDriver.hpp"
#include "CminusScanner.hpp"

using namespace std;

Cminus::Driver::Driver()
    : trace_parsing(false),
      trace_scanning(false)
{
    Types.insert(Types.begin(), "int");
    Types.push_back("string");
}

int Cminus::Driver::Parse(const string& fileName, istream* inputStream, ostream* outputStream)
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

int Cminus::Driver::GetTypeID(string&& typeName)
{
    for(int i = 0; i < Types.size(); i += 1)
        if(Types[i] == typeName)
            return i;
    throw string("Unrecognized type name \"").append(typeName).append("\".");
}

Cminus::parser::symbol_type Cminus::Driver::NextSymbol()
{
    return scanner->yylex(*this);
}
