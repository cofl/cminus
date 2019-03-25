#ifndef CminusDriver_HPP
    /* Preamble */
    #define CminusDriver_HPP

    #include <string>
    #include <iostream>
    #include <map>
    #include <vector>
    #include "CminusParser.hpp"
    #include "ast/GlobalStatementListASTNode.hpp"
    #include "structures/SymbolTable.hpp"

    /* Definitions */
namespace Cminus
{
    using namespace std;

    /* Driver Class */
    class Driver
    {
        public:
            Driver();

            int Parse(const std::string& fileName, std::istream* inputStream, std::ostream* outputStream);
            parser::symbol_type NextSymbol();

            location Location;
            std::string FileName;
            bool trace_parsing;
            bool trace_scanning;

            std::vector<std::string> Types;
            GlobalStatementListASTNode* Root;
            int GetTypeID(std::string&& typeName);
            void Process(std::ostream& outputStream);
        private:
            Scanner* scanner;
    };
}

#endif
