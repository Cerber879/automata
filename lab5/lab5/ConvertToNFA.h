#pragma once

#include <map>
#include <set>
#include <queue>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>

#include "Constants.h"

class ConvertToNFA
{
public:
    NFA ParseRegexToNFA(const string& regex);
    string readRegexFromFile(const string& filename);

private:
    void MakingTransition(int count, map<int, map<char, set<int>>>& states, int& startID, char& lastSymbol, int& currID, char& currentSymbol);
    void ProcesingOperator(queue<char>& symbol, map<int, map<char, set<int>>>& states, int& currID, char& lastSymbol, bool& needinessBegining, int startID, bool isLastCharOperator);
    void BuildEndTransitions(vector<int>& endStates, map<int, map<char, set<int>>>& states, int endState);

    char voidTransition = '@'; 
    int endStateID = -1;    
};
