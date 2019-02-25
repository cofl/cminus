#ifndef CminusDriver_HPP
    /* Preamble */
    #define CminusDriver_HPP
    #include <string>
    #include <iostream>
    #include <unordered_map>
    #include "CminusParser.hpp"

    /* Definitions */
namespace Cminus
{
    /* Driver Class */
    class Driver
    {
        public:
            Driver();

            int Parse(const std::string& fileName);
            parser::symbol_type NextSymbol();

            location Location;
            std::string FileName;
            bool trace_parsing;
            bool trace_scanning;
            std::unordered_map<std::string, int*> Variables;
        private:
            Scanner* scanner;
    };
}

#endif
