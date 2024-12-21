#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

const char OPENING_STATE_NAME = '<';
const char FINISHING_STATE_NAME = '>';
const char TRANSPARENT = ' ';
const char TAB = '\t';
const char DELIMITER = '|';
const char DELIMITER_DKA = ';';
const char DELIMITER_NAME_STATES = ',';

const string VOID_TRANSITION = "-";
const string ARROW = "->";

const string DEFAULT_NAME_STATE = "H";
const string Q_NAME_STATE = "q";

const string MARK_END_STATE = "(end)";

const string STRUCT_INPUT = "\n\nCheck struct input file:\n<S> -> a<A> | b<B>\n<P> -> <N>.\n<A> -> a<A> | b<S>\n<B> -> a<S> | b\n\nS, A, B - states, a and b input symbols";
const string STRUCT_INPUT_DKA = "\n\nCheck struct input file:\n  ;S   	;A      	;B\na;-    	;A,B 	;B\nb;A    	;-        	;S\n\nS, A, B - states, a and b input symbols";

struct NKAResult
{
    map<string, map<string, vector<string>>> nka;
    vector<string> orderedStates;
    vector<string> orderedSymbols;
};

struct DKAResult
{
    map<string, map<string, string>> dka;
    vector<string> orderedStates;
};

enum class StateNameType
{
    Full,
    BetweenDelimiters
};

enum class GrammarType
{
    Right,
    Left,
    Undefined
};

struct CustomOrder
{
    string startState;
    string endState;

    CustomOrder(const string& start, const string& end) : startState(start), endState(end) {}

    bool operator()(const string& a, const string& b) const {
        if (a == endState) return false;
        if (b == endState) return true;

        if (a == startState) return true;
        if (b == startState) return false;

        return a < b;
    }
};