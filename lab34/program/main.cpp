#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "ConverterToNKA.h"
#include "Constants.h"
#include "ReaderNKA.h"
#include "ConvertorToDKA.h"

using namespace std;

const int COUNT_ARGS = 4;
const string NKA = "nka";
const string DKA = "dka";

const string PATH_INPUT_FILE = "C:/Users/User/source/repos/automata/lab34/input.txt";
const string PATH_OUTPUT_FILE = "C:/Users/User/source/repos/automata/lab34/output.txt";

struct Args
{
    std::string inputFileName;
    std::string typeTable;
    std::string outputFileName;
};

Args ParseArgs(int argc, char* argv[])
{
    if (argc != COUNT_ARGS)
    {
        throw runtime_error("Invalid arguments count\nUsage: program.exe <input file name> <type input file(nka/dka)> <output file name>\n");
    }

    Args args;
    args.inputFileName = argv[1];
    args.typeTable = argv[2];
    args.outputFileName = argv[3];

    return args;
}

int main(int argc, char* argv[])
{
    try
    {
        ConverterToNKA converterToNKA;
        NKAResult tableNKA = converterToNKA.ConversionToNKA(PATH_INPUT_FILE);

        ConvertorToDKA convertorToDKA;
        DKAResult resultDKA = convertorToDKA.ConversionToDKA(tableNKA, tableNKA.orderedStates[0]);
        convertorToDKA.WriteToFileDKA(PATH_OUTPUT_FILE, tableNKA, resultDKA);
        //Args args = ParseArgs(argc, argv);

        //if (args.typeTable == NKA)
        //{
        //    ConverterToNKA converterToNKA;

        //    NKAResult tableNKA = converterToNKA.ConversionToNKA(args.inputFileName);
        //    converterToNKA.WriteToFileNKA(args.outputFileName, tableNKA);
        //}
        //else if (args.typeTable == DKA)
        //{
        //    ReaderNKA readerNKA;
        //    NKAResult tableNKA = readerNKA.ReadNKA(args.inputFileName);

        //    ConvertorToDKA convertorToDKA;
        //    DKAResult resultDKA = convertorToDKA.ConversionToDKA(tableNKA, tableNKA.orderedStates[0]);
        //    convertorToDKA.WriteToFileDKA(args.outputFileName, tableNKA, resultDKA);
        //}
        //else
        //{
        //    throw runtime_error("Invalid file type.Please ensure the file is of type 'nka' or 'dka'.");
        //}
    }
    catch (const runtime_error& e)
    {
        cerr << "Runtime error occurred: " << e.what() << endl;
    }
    catch (const exception& e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "An unknown error occurred." << endl;
    }

    return 0;
}
