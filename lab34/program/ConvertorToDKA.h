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

class ConvertorToDKA
{
public:
    DKAResult ConversionToDKA(NKAResult& result, const string& startNameState);
    void WriteToFileDKA(const string& filename, NKAResult& tableNKA, DKAResult& resultDKA);
private:
    void AddAdjustmentState(int countIteration, std::vector<std::string>& currentNameState, std::string& endState, std::map<std::vector<std::string>, std::string>& adjustmentStates);
};

