#include "ReaderNKA.h"

void ReaderNKA::SkipSpaces(const string& line, size_t& pos)
{
    while (pos < line.size() && (isspace(line[pos]) || line[pos] == TAB))
    {
        pos++;
    }
}

string ReaderNKA::ReadNameStateDKA(const string& line, size_t& pos)
{
    SkipSpaces(line, pos);

    string nameState;
    while (pos < line.size() && line[pos] != DELIMITER_DKA && line[pos] != TAB && line[pos] != TRANSPARENT && line[pos] != DELIMITER_NAME_STATES)
    {
        nameState += line[pos];
        pos++;
    }

    if (nameState.empty())
    {
        throw runtime_error("State name cannot be empty" + STRUCT_INPUT_DKA);
    }

    return nameState;
}

vector<string> ReaderNKA::ReadNameStates(const string& line)
{
    vector<string> orderedStates;
    size_t pos = 0;

    SkipSpaces(line, pos);

    while (pos < line.size())
    {
        if (line[pos] != DELIMITER_DKA)
        {
            throw runtime_error("Failed to read state name, check delimiters between states" + STRUCT_INPUT_DKA);
        }
        pos++;

        string nameState = ReadNameStateDKA(line, pos);
        orderedStates.push_back(nameState);

        SkipSpaces(line, pos);
    }

    if (orderedStates.empty())
    {
        throw runtime_error("Head table cannot be empty" + STRUCT_INPUT_DKA);
    }

    return orderedStates;
}

string ReaderNKA::ReadTransitionSymbolDKA(const string& line, size_t& pos)
{
    string symbol;

    while (pos < line.size() && line[pos] != DELIMITER_DKA && !isspace(line[pos]))
    {
        symbol += line[pos];
        pos++;
    }

    if (symbol.empty())
    {
        throw runtime_error("Failed to read transition symbol from stream" + STRUCT_INPUT);
    }

    SkipSpaces(line, pos);

    return symbol;
}

NKAResult ReaderNKA::ReadNKA(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Failed to open file: " + filename);
    }

    string line;

    map<string, map<string, vector<string>>> nka;
    vector<string> uniqueSymbols;

    getline(file, line);
    vector<string> orderedStates = ReadNameStates(line);

    while (getline(file, line))
    {
        int countCurrentState = 0;
        size_t pos = 0;
        SkipSpaces(line, pos);

        string symbol = ReadTransitionSymbolDKA(line, pos);
        uniqueSymbols.push_back(symbol);

        while (pos < line.size())
        {
            if (line[pos] != DELIMITER_DKA)
            {
                throw runtime_error("Failed to read state name, check delimiters between states" + STRUCT_INPUT_DKA);
            }
            pos++;

            string nameState;
            while (pos < line.size() && line[pos] != DELIMITER_DKA)
            {
                nameState = ReadNameStateDKA(line, pos);
                nka[orderedStates[countCurrentState]][symbol].push_back(nameState);
                SkipSpaces(line, pos);
                if (pos < line.size() && line[pos] != DELIMITER_DKA && line[pos] != DELIMITER_NAME_STATES)
                {
                    throw runtime_error("Failed to read state name, check delimiters between states" + STRUCT_INPUT_DKA);
                }
                else if (line[pos] == DELIMITER_NAME_STATES)
                {
                    pos++;
                }
            }
            countCurrentState++;
        }
    }

    sort(uniqueSymbols.begin(), uniqueSymbols.end());

    return NKAResult{ nka, orderedStates, uniqueSymbols };
}