#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <cctype>
#include <unordered_set>
#include <set>
#include <vector>
#include <algorithm>

#include "Constants.h"

class ConverterToNKA
{
public:
	NKAResult ConversionToNKA(const string& filename);
	void WriteToFileNKA(const string& filename, NKAResult& tableNKA);

private:
	void SkipSpaces(const string& line, size_t& pos);
	void SkipToDelimiter(const string& line, size_t& pos);

	string ReadStateNameBetweenDelimiters(const string& line, size_t& pos);
	string ProcessingReadState(const string& line, size_t& pos);
	string ReadTransitionSymbol(const string& line, size_t& pos);

	void CheckArrow(const string& line, size_t& pos);
	GrammarType DetermineGrammarType(ifstream& file);

	void AddVoidTransitions(unordered_set<string>& uniqueSymbols, unordered_set<string>& uniqueStates, string& firstState, map<string, map<string, vector<string>>>& nka);

	string ProcessingReadTransitionSymbol(string& line, size_t& pos, unordered_set<string>& uniqueSymbols);
	void ProcessingReadChildState(string& line, size_t& pos, string& nameChildState, unordered_set<string>& uniqueStates);
};

