#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>
#include <unistd.h>
#include "CminusDriver.hpp"
#include "CminusParser.hpp"

using namespace std;

static const char* HELP_MESSAGE =
    "Usage:\n"
    " cmc [options] <file>\n"
    "\n"
    "Compiles Cminus programs.\n"
    "\n"
    "Options:\n"
    " -d[ps]      Display extended debugging information for the\n"
    "             [p]arser or [s]canner (both if none specified).\n"
    " -h          Display this help.\n"
    " -o          Override the output file name. Use \"-\" to write to stdout."
    ;

int main(int argc, char** argv)
{
    Cminus::Driver driver;
    struct {
        string inputFileName;
        string outputFileName;
        bool openedInputFile;
        bool openedOutputFile;
    } options = {"", ""};

    int c;
    opterr = 0;
    while((c = getopt(argc, argv, "d::o:h")) != -1)
    {
        switch(c)
        {
            case 'd':
                if(optarg == NULL)
                {
                    driver.trace_parsing = true;
                    driver.trace_scanning = true;
                } else
                {
                    driver.trace_parsing = string(optarg).find('p') != string::npos;
                    driver.trace_scanning = string(optarg).find('s') != string::npos;
                }
                break;
            case 'o':
                options.outputFileName = optarg;
                break;
            case 'h':
                cerr << HELP_MESSAGE << endl;
                return 0;
            case '?':
                if(optopt == 'o')
                    cerr << "Option \"-" << optopt << "\" requires an argument." << endl;
                else
                    cerr << "Unrecognized option \"-" << optopt << "\"." << endl;
                cerr << HELP_MESSAGE << endl;
                return 1;
            default:
                abort();
        }
    }

    if(optind < argc)
        options.inputFileName = argv[optind];

    istream* inputStream = &cin;
    ostream* outputStream = &cout;

    // if we have a file and it isn't stdin.
    if(options.inputFileName != "-" && !options.inputFileName.empty())
    {
        inputStream = new ifstream(options.inputFileName);
        options.openedInputFile = true;
        if(!inputStream->good())
        {
            cerr << "Cannot open input file: " << options.inputFileName << endl;
            return 1;
        }
    }

    if(options.inputFileName == "-")
    {
        options.inputFileName = "";
        if(options.outputFileName.empty())
            options.outputFileName = "-";
    }

    // if we have a filename and we need to generate an output file name.
    if(!options.inputFileName.empty() && options.outputFileName.empty())
        // generate the name
        options.outputFileName = options.inputFileName.substr(0, options.inputFileName.rfind('.')) + ".s";

    if(options.outputFileName.empty())
    {
        cerr << "An input or output file must be provided." << endl
             << HELP_MESSAGE << endl;
        return 1;
    } else if(options.outputFileName != "-")
    {
        outputStream = new ofstream(options.outputFileName);
        options.openedOutputFile = true;
        if(!outputStream->good())
        {
            cerr << "Cannot open output file: " << options.outputFileName << endl;
            return 1;
        }
    }

    int ret = driver.Parse(options.inputFileName, inputStream, outputStream);
    driver.Process(*outputStream);

    if(options.openedInputFile)
    {
        ((ifstream*)inputStream)->close();
        delete inputStream;
    }

    if(options.openedOutputFile)
    {
        ((ofstream*)outputStream)->close();
        delete outputStream;
    }

    return ret;
}
