#include "ConvertToNFA.h"

string ConvertToNFA::readRegexFromFile(const string& filename)
{
    ifstream file(filename);

    string regex;
    string str;

    while (file >> str)
    {
        if (str.find(' ') == string::npos && str.find('\t') == string::npos)
        {
            regex += str;
        }
    }
    cout << regex << '\n';
    file.close();
    return regex;
}

void ConvertToNFA::MakingTransition(int count, std::map<int, std::map<char, std::set<int>>>& states,
    int& startID, char& lastSymbol, int& currID, char& currentSymbol)
{
    if (count == 1)
    {
        states[startID][lastSymbol].insert(currID + 1);
        cout << startID << "--" << lastSymbol << "-->" << currID + 1 << '\n';
        states[currID + 1][currentSymbol].insert(currID + 1);
        cout << currID + 1 << "--" << currentSymbol << "-->" << currID + 1 << '\n';
    }
    else
    {
        states[startID][currentSymbol].insert(currID + 1);
        cout << startID << "--" << currentSymbol << "-->" << currID + 1 << '\n';
    }
}

void ConvertToNFA::ProcesingOperator(queue<char>& symbol, map<int, map<char, set<int>>>& states,
    int& currID, char& lastSymbol, bool needinessBegining, int startID, bool isLastCharOperator)
{
    int count = isLastCharOperator ? symbol.size() : 0;

    while (!symbol.empty())
    {
        char currentSymbol = symbol.front();
        symbol.pop();

        if (needinessBegining)
        {
            MakingTransition(count, states, startID, lastSymbol, currID, currentSymbol);
            needinessBegining = false;
        }
        else
        {
            MakingTransition(count, states, currID, lastSymbol, currID, currentSymbol);
        }

        currID++;
        count--;
    }
}

void ConvertToNFA::BuildEndTransitions(std::vector<int>& endStates, std::map<int, std::map<char, std::set<int>>>& states, int endState)
{
    for (int stateID : endStates)
    {
        states[stateID][voidTransition].insert(endState);
        cout << stateID << "--" << voidTransition << "-->" << endState << '\n';
    }

    endStates.clear();
}

NFA ConvertToNFA::ParseRegexToNFA(const string& regex)
{
    map<int, map<char, set<int>>> states;
    set<char> orderedSymbols;
    int currID = 0;

    bool needinessBegining = false;
    stack<int> startState;
    startState.push(currID);

    stack<vector<int>> endStates;
    endStates.push({ });
    queue<char> symbol;

    for (int i = 0; i < regex.size(); i++)
    {
        if (regex[i] == voidTransition)
        {
            symbol.push(regex[i]);
        }
        if (isalnum(regex[i]))
        {
            orderedSymbols.insert(regex[i]);
            symbol.push(regex[i]);
        }
        else if (regex[i] == '*')
        {
            ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), true);
            if (needinessBegining) needinessBegining = false;
        }
        else if (regex[i] == '+')
        {
            ProcesingOperator(symbol, states, currID, symbol.back(), needinessBegining, startState.top(), true);
            if (needinessBegining) needinessBegining = false;
        }
        else if (regex[i] == '|')
        {
            ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), false);
            endStates.top().push_back(currID);
            needinessBegining = true;
        }
        else if (regex[i] == '(')
        {
            endStates.push({ });
            ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), false);

            symbol.push(voidTransition);
            ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), false);

            startState.push(currID);
        }
        else if (regex[i] == ')')
        {
            ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), false);
            needinessBegining = false;
            endStates.top().push_back(currID);

            if (regex[i + 1] == '*')
            {
                BuildEndTransitions(endStates.top(), states, startState.top());
                states[startState.top()][voidTransition].insert(currID + 1);
                cout << startState.top() << "--" << voidTransition << "-->" << currID + 1 << '\n';
                currID++;
                i++;
            }
            else if (regex[i + 1] == '+')
            {
                BuildEndTransitions(endStates.top(), states, currID + 1);
                states[currID][voidTransition].insert(startState.top());
                cout << currID << "--" << voidTransition << "-->" << startState.top() << '\n';
                currID++;
                i++;
            }
            else
            {
                BuildEndTransitions(endStates.top(), states, currID + 1);
                currID++;
            }

            startState.pop();
            endStates.pop();
        }
    }

    ProcesingOperator(symbol, states, currID, voidTransition, needinessBegining, startState.top(), false);
    endStates.top().push_back(currID);
    BuildEndTransitions(endStates.top(), states, endStateID);

    return NFA{ states, orderedSymbols, currID + 1 };
}
