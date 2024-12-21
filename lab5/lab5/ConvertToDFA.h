#pragma once

#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <fstream>
#include <cctype>
#include <unordered_set>
#include <set>
#include <vector>
#include <algorithm>

#include "Constants.h"

class ConvertToDFA
{
public:
    DFA Convert(NFA& nfa, map<int, set<int>>& epsilonClosures);
    void WriteToFileDFA(const string& filename, NFA& nfa, DFA& dfa);

private:
    void AddAdjustmentState(int countIteration, vector<int>& currentNameState, 
        map<vector<int>, string>& adjustmentStates);

    string nameState = "q";
    string voidTransition = "-";
    int startStateID = 0;
    int endStateID = -1;
};

