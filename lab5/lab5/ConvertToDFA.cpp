#include "ConvertToDFA.h"

void ConvertToDFA::AddAdjustmentState(int countIteration, vector<int>& currentNameState, map<vector<int>, string>& adjustmentStates)
{
    string adjustmentNameState = nameState + to_string(countIteration);

    for (const int& childNameState : currentNameState)
    {
        if (childNameState == endStateID)
        {
            adjustmentNameState += "(end)";
            break;
        }
    }

    adjustmentStates[currentNameState] = adjustmentNameState;
}

DFA ConvertToDFA::Convert(NFA& nfa, map<int, set<int>>& epsilonClosures)
{
    map<string, map<char, string>> dfa;
    set<vector<int>> dfaStates;
    vector<string> orderedStates;
    queue<vector<int>> stateQueue;

    vector<int> startState(epsilonClosures[startStateID].begin(), epsilonClosures[startStateID].end());

    stateQueue.push(startState);
    dfaStates.insert(startState);

    map<vector<int>, string> adjustmentStates;
    adjustmentStates[startState] = nameState + to_string(startStateID);
    orderedStates.push_back(nameState + to_string(startStateID));
    int countIteration = 1;

    while (!stateQueue.empty())
    {
        vector<int> currentNameState = stateQueue.front();
        stateQueue.pop();

        for (const char& symbol : nfa.orderedSymbols)
        {
            unordered_set<int> uniqueStates;
            if (currentNameState == vector<int>{ 3, 10 } && symbol == 'c')
            {
                cout <<  "";
            }
            for (const int& childNameState : currentNameState)
            {
                if (childNameState != endStateID && nfa.table.at(childNameState).find(symbol) != nfa.table.at(childNameState).end())
                {
                    for (const int& nextState : nfa.table.at(childNameState).at(symbol))
                    {
                        for (const int& state : epsilonClosures[nextState])
                        {
                            uniqueStates.insert(state);
                        }
                    }
                }
            }

            vector<int> newNameState(uniqueStates.begin(), uniqueStates.end());
            sort(newNameState.begin(), newNameState.end());

            if (newNameState.empty())
            {
                dfa[adjustmentStates[currentNameState]][symbol] = "-";
                cout << adjustmentStates[currentNameState] << " - ";
                for (const auto& state : currentNameState)
                {
                    cout << state << ',';
                }
                cout << " --" << symbol << "--> " << "-" << endl;
            }
            else
            {
                if (dfaStates.find(newNameState) == dfaStates.end())
                {
                    AddAdjustmentState(countIteration, newNameState, adjustmentStates);
                    countIteration++;

                    stateQueue.push(newNameState);
                    dfaStates.insert(newNameState);
                    orderedStates.push_back(adjustmentStates[newNameState]);
                }

                dfa[adjustmentStates[currentNameState]][symbol] = adjustmentStates[newNameState];
                cout << adjustmentStates[currentNameState] << " - ";
                for (const auto& state : currentNameState)
                {
                    cout << state << ',';
                }
                cout << " --" << symbol << "--> " << adjustmentStates[newNameState] << " - ";
                for (const auto& state : newNameState)
                {
                    cout << state << ',';
                }
                cout << endl;
            }
        }
    }

    return DFA({ dfa, orderedStates });
}

void ConvertToDFA::WriteToFileDFA(const string& filename, NFA& nfa, DFA& dfa)
{
    ofstream file(filename);

    for (const string& state : dfa.orderedStates)
    {
        file << ';' << state << ' ';
    }
    file << '\n';

    for (const char& symbol : nfa.orderedSymbols)
    {
        file << symbol;

        for (const string& transition : dfa.orderedStates)
        {
            file << ';' << dfa.table[transition][symbol] << ' ';
        }
        file << '\n';
    }
}