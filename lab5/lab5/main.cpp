#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <map>
#include <string>
#include <fstream>

#include "Constants.h"
#include "ConvertToNFA.h"
#include "ComputeEpsilonClosures.h"
#include "ConvertToDFA.h"

int main()
{
    ConvertToNFA convertatorNFA;

    string regex = convertatorNFA.readRegexFromFile(INPUT_FILE);
    NFA nfa = convertatorNFA.ParseRegexToNFA(regex);


    ComputeEpsilonClosures compute(nfa);

    map<int, set<int>> epsilonClosures = compute.Compute();
    compute.PrintClosures();


    ConvertToDFA convertatorDFA;

    DFA dfa = convertatorDFA.Convert(nfa, epsilonClosures);
    convertatorDFA.WriteToFileDFA(OUTPUT_FILE, nfa, dfa);

    return 0;
}
