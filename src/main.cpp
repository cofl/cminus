#include <cstdio>
#include <string>
#include "CminusDriver.hpp"
#include "CminusParser.hpp"

static void init(std::string inputFileName)
{
    if(inputFileName.empty() || inputFileName == "-")
        return;
    auto outputFile = inputFileName.substr(0, inputFileName.rfind('.')) + ".s";
    stdout = freopen(outputFile.c_str(),"w",stdout);
    if (stdout == NULL)
    {
        std::cerr << "Error: Could not open file " << outputFile << "\n";
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    auto inputFileName = argc > 1 ? argv[1] : "";
    Cminus::Driver driver;
    //driver.trace_parsing = true;
    //driver.trace_scanning = true;
    init(inputFileName);
    return driver.Parse(inputFileName);
}
