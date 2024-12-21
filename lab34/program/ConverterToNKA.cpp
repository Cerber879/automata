#include "ConverterToNKA.h"

void ConverterToNKA::SkipSpaces(const string& line, size_t& pos)
{
    while (pos < line.size() && (isspace(line[pos]) || line[pos] == TAB))
    {
        pos++;
    }
}

void ConverterToNKA::SkipToDelimiter(const string& line, size_t& pos)
{
    while (pos < line.size() && line[pos] != DELIMITER)
    {
        if (!isspace(line[pos]))
        {
            throw runtime_error("Not correct transition" + STRUCT_INPUT);
        }

        pos++;
    }

    pos++;
}

string ConverterToNKA::ReadStateNameBetweenDelimiters(const string& line, size_t& pos)
{
    string nameState;

    if (line[pos] != OPENING_STATE_NAME)
    {
        throw runtime_error("Failed to read opening state name character from stream" + STRUCT_INPUT);
    }
    pos++;

    while (pos < line.size() && line[pos] != FINISHING_STATE_NAME)
    {
        if (!isspace(line[pos]))
        {
            nameState += line[pos];
        }
        pos++;
    }

    if (pos >= line.size() || line[pos] != FINISHING_STATE_NAME)
    {
        throw runtime_error("Failed to read finishing state name character from stream" + STRUCT_INPUT);
    }
    pos++;

    return nameState;
}

string ConverterToNKA::ProcessingReadState(const string& line, size_t& pos)
{
    string nameState = ReadStateNameBetweenDelimiters(line, pos);

    if (nameState.empty() || nameState.find(DELIMITER) != string::npos || nameState.find(OPENING_STATE_NAME) != string::npos)
    {
        throw runtime_error("State name cannot be empty" + STRUCT_INPUT);
    }

    return nameState;
}

void ConverterToNKA::CheckArrow(const string& line, size_t& pos)
{
    while (pos < line.size() && isspace(line[pos]))
    {
        pos++;
    }

    if (line.substr(pos, ARROW.size()) != ARROW)
    {
        throw runtime_error("Failed to read arrow symbol from stream" + STRUCT_INPUT);
    }

    pos += ARROW.size();
}

string ConverterToNKA::ReadTransitionSymbol(const string& line, size_t& pos)
{
    string symbol;

    while (pos < line.size() && line[pos] != OPENING_STATE_NAME && line[pos] != DELIMITER && !isspace(line[pos]))
    {
        symbol += line[pos];
        pos++;
    }

    if (symbol.empty())
    {
        throw runtime_error("Failed to read transition symbol from stream" + STRUCT_INPUT);
    }

    if (symbol.find(OPENING_STATE_NAME) != string::npos || symbol.find(FINISHING_STATE_NAME) != string::npos)
    {
        throw runtime_error("Not correct transition: " + STRUCT_INPUT);
    }

    SkipSpaces(line, pos);

    return symbol;
}

GrammarType ConverterToNKA::DetermineGrammarType(ifstream& file)
{
    string line;

    while (getline(file, line))
    {
        size_t pos = 0;

        SkipSpaces(line, pos);
        string nameState = ProcessingReadState(line, pos);

        CheckArrow(line, pos);
        size_t tempPos = pos;
        string nameChildState;
        string symbol;

        while (tempPos < line.size())
        {
            SkipSpaces(line, tempPos);

            if (line[tempPos] == OPENING_STATE_NAME)
            {
                nameChildState = ReadStateNameBetweenDelimiters(line, tempPos);
                symbol = ReadTransitionSymbol(line, tempPos);
                return GrammarType::Left;
            }
            else
            {
                symbol = ReadTransitionSymbol(line, tempPos);
                if (line[tempPos] == OPENING_STATE_NAME)
                {
                    nameChildState = ReadStateNameBetweenDelimiters(line, tempPos);
                    return GrammarType::Right;
                }
            }

            SkipToDelimiter(line, tempPos);
        }
    }
}

void ConverterToNKA::AddVoidTransitions(unordered_set<string>& uniqueSymbols, unordered_set<string>& uniqueStates,
    string& firstState, map<string, map<string, vector<string>>>& nka)
{
    if (uniqueStates.find(DEFAULT_NAME_STATE) == uniqueStates.end())
    {
        uniqueStates.insert(DEFAULT_NAME_STATE);
    }

    if (uniqueStates.find(firstState) == uniqueStates.end())
    {
        uniqueStates.insert(firstState);
    }

    for (const auto& symbol : uniqueSymbols)
    {
        for (const auto& state : uniqueStates)
        {
            if (nka[state].find(symbol) == nka[state].end())
            {
                nka[state][symbol].push_back("-");
            }
        }
    }
}

string ConverterToNKA::ProcessingReadTransitionSymbol(string& line, size_t& pos, unordered_set<string>& uniqueSymbols)
{
    string symbol = ReadTransitionSymbol(line, pos);
    uniqueSymbols.insert(symbol);
    return symbol;
}

void ConverterToNKA::ProcessingReadChildState(string& line, size_t& pos, string& nameChildState, unordered_set<string>& uniqueStates)
{
    if (line[pos] == OPENING_STATE_NAME)
    {
        nameChildState = ProcessingReadState(line, pos);
        uniqueStates.insert(nameChildState);
    }
}

NKAResult ConverterToNKA::ConversionToNKA(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Failed to open file: " + filename);
    }
    else if (file.peek() == std::ifstream::traits_type::eof())
    {
        throw std::runtime_error("File is empty: " + filename);
    }

    GrammarType grammarType = GrammarType::Undefined;
    grammarType = DetermineGrammarType(file);

    file.clear(); 
    file.seekg(0, std::ios::beg);

    string line;
    map<string, map<string, vector<string>>> nka;
    string firstState;

    unordered_set<string> uniqueSymbols;
    unordered_set<string> uniqueStates;

    while (getline(file, line))
    {
        size_t pos = 0;

        SkipSpaces(line, pos);
        string nameState = ProcessingReadState(line, pos);

        map<string, vector<string>> states;

        CheckArrow(line, pos);
        if (firstState == "") firstState = nameState;

        while (pos < line.size())
        {
            string nameChildState = DEFAULT_NAME_STATE;
            string symbol;

            SkipSpaces(line, pos);

            if (grammarType == GrammarType::Right)
            {
                symbol = ProcessingReadTransitionSymbol(line, pos, uniqueSymbols);
                ProcessingReadChildState(line, pos, nameChildState, uniqueStates);
                states[symbol].push_back(nameChildState);
            }
            else
            {
                ProcessingReadChildState(line, pos, nameChildState, uniqueStates);
                symbol = ProcessingReadTransitionSymbol(line, pos, uniqueSymbols);
                nka[nameChildState][symbol].push_back(nameState);
            }

            SkipToDelimiter(line, pos);
        }
        if (grammarType == GrammarType::Right) nka[nameState] = states;
    }

    AddVoidTransitions(uniqueSymbols, uniqueStates, firstState, nka);

    vector<string> orderedStates(uniqueStates.begin(), uniqueStates.end());
    if (grammarType == GrammarType::Right)
    {
        sort(orderedStates.begin(), orderedStates.end(), CustomOrder(firstState, DEFAULT_NAME_STATE));
    }
    else
    {
        sort(orderedStates.begin(), orderedStates.end(), CustomOrder(DEFAULT_NAME_STATE, firstState));
    }

    vector<string> orderedSymbols(uniqueSymbols.begin(), uniqueSymbols.end());
    sort(orderedSymbols.begin(), orderedSymbols.end());

    return NKAResult({ nka, orderedStates, orderedSymbols });
}

void ConverterToNKA::WriteToFileNKA(const string& filename, NKAResult& tableNKA)
{
    ofstream file(filename);

    for (const auto& state : tableNKA.orderedStates)
    {
        file << DELIMITER_DKA << state << TRANSPARENT;
    }
    file << '\n';

    for (const auto& symbol : tableNKA.orderedSymbols)
    {
        file << symbol;

        for (const auto& transition : tableNKA.orderedStates)
        {
            file << DELIMITER_DKA << tableNKA.nka[transition][symbol][0];
            for (int i = 1; i < tableNKA.nka[transition][symbol].size(); i++)
            {
                file << DELIMITER_NAME_STATES << tableNKA.nka[transition][symbol][i];
            }
            file << TRANSPARENT;
        }
        file << '\n';
    }

}