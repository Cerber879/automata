#include "ConvertorToDKA.h"

void ConvertorToDKA::AddAdjustmentState(int countIteration, std::vector<std::string>& currentNameState, std::string& endState, std::map<std::vector<std::string>, std::string>& adjustmentStates)
{
    string adjustmentNameState = Q_NAME_STATE + to_string(countIteration);
    for (const string& childNameState : currentNameState)
    {
        if (childNameState == endState)
        {
            adjustmentNameState += MARK_END_STATE;
            break;
        }
    }
    adjustmentStates[currentNameState] = adjustmentNameState;
}

DKAResult ConvertorToDKA::ConversionToDKA(NKAResult& result, const string& startNameState)
{
    map<string, map<string, string>> dka;
    set<vector<string>> dkaStates;
    vector<string> orderedStates;
    queue<vector<string>> stateQueue;

    vector<string> startState = { startNameState };
    string endState = result.orderedStates.back();

    stateQueue.push(startState);
    dkaStates.insert(startState);

    map<vector<string>, string> adjustmentStates;
    adjustmentStates[startState] = Q_NAME_STATE + to_string(0);
    orderedStates.push_back(Q_NAME_STATE + to_string(0));
    int countIteration = 1;

    while (!stateQueue.empty())
    {
        vector<string> currentNameState = stateQueue.front();
        stateQueue.pop();

        for (const string& symbol : result.orderedSymbols)
        {
            unordered_set<string> uniqueStates;

            for (const string& childNameState : currentNameState)
            {
                for (const string& state : result.nka[childNameState][symbol])
                {
                    if (state != VOID_TRANSITION)
                    {
                        uniqueStates.insert(state);
                    }
                }
            }

            vector<string> newNameState(uniqueStates.begin(), uniqueStates.end());
            sort(newNameState.begin(), newNameState.end());

            if (newNameState.empty())
            {
                dka[adjustmentStates[currentNameState]][symbol] = VOID_TRANSITION;
            }
            else
            {
                if (dkaStates.find(newNameState) == dkaStates.end())
                {
                    AddAdjustmentState(countIteration, newNameState, endState, adjustmentStates);
                    countIteration++;

                    stateQueue.push(newNameState);
                    dkaStates.insert(newNameState);
                    orderedStates.push_back(adjustmentStates[newNameState]);
                }

                dka[adjustmentStates[currentNameState]][symbol] = adjustmentStates[newNameState];
            }
        }
    }

    return DKAResult({ dka, orderedStates });
}

void ConvertorToDKA::WriteToFileDKA(const string& filename, NKAResult& tableNKA, DKAResult& resultDKA)
{
    ofstream file(filename);

    for (const auto& state : resultDKA.orderedStates)
    {
        file << DELIMITER_DKA << state << TRANSPARENT;
    }
    file << '\n';

    for (const auto& symbol : tableNKA.orderedSymbols)
    {
        file << symbol;

        for (const auto& transition : resultDKA.orderedStates)
        {
            file << DELIMITER_DKA << resultDKA.dka[transition][symbol] << TRANSPARENT;
        }
        file << '\n';
    }
}