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
    // nop
}

int Cminus::Driver::Parse(const string& fileName)
{
    FileName = fileName;
    Location.initialize (& (string&) fileName);

    ifstream file;
    if (fileName.empty() || fileName == "-")
        scanner = new Scanner(std::cin);
    else
    {
        file.open(fileName, ifstream::in);
        if(!file.good())
        {
            cerr << "Cannot open " << fileName << ": " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        scanner = new Scanner(file);
    }
    scanner->set_debug(trace_scanning);

    Cminus::parser parser(*this);
    parser.set_debug_level (trace_parsing);
    int res = parser.parse();

    if(file.is_open())
        file.close();

    return res;
}

Cminus::parser::symbol_type Cminus::Driver::NextSymbol()
{
    return scanner->yylex(*this);
}
