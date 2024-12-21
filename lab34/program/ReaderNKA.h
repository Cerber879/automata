#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cctype>
#include <algorithm>

#include "Constants.h"

class ReaderNKA
{
public:
	NKAResult ReadNKA(const string& filename);

private:
	void SkipSpaces(const string& line, size_t& pos);
	string ReadNameStateDKA(const string& line, size_t& pos);
	vector<string> ReadNameStates(const string& line);
	string ReadTransitionSymbolDKA(const string& line, size_t& pos);
};

