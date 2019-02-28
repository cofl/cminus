#ifndef CminusDriver_HPP
    /* Preamble */
    #define CminusDriver_HPP
    #include <string>
    #include <iostream>
    #include <unordered_map>
    #include <vector>
    #include "CminusParser.hpp"

    /* Definitions */
namespace Cminus
{
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
            std::unordered_map<std::string, int*> Variables;

            int GetTypeID(std::string&& typeName);
        private:
            Scanner* scanner;
    };
}

#endif
