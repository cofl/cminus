#include "CminusDriver.hpp"

CminusDriver::CminusDriver()
    : trace_parsing(false), trace_scanning(false)
{
    // nop
}

int CminusDriver::Parse(const std::string& fileName)
{
    FileName = fileName;
    Location.initialize (& (std::string&) fileName);
    scan_begin();
    Cminus::parser parser(*this);
    parser.set_debug_level (trace_parsing);
    int res = parser.parse();
    scan_end();
    return res;
}
