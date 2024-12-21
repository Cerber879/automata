#pragma once

#include <iostream>
#include <string>
#include <set>
#include <map>

using namespace std;

const string INPUT_FILE = "C:/Users/User/source/repos/automata/lab5/input.txt";
const string OUTPUT_FILE = "C:/Users/User/source/repos/automata/lab5/output.txt";

struct NFA
{
    map<int, map<char, set<int>>> table;
    set<char> orderedSymbols;
    int countStates;
};

struct DFA
{
    map<string, map<char, string>> table;
    vector<string> orderedStates;
};