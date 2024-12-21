#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <stack>

#include "Constants.h"

class ComputeEpsilonClosures
{
public:
    ComputeEpsilonClosures(const NFA& nfa) : nfa(nfa) {}

    map<int, set<int>> Compute();
    void PrintClosures() const;

private:
    void ComputeClosure(int state);

    const NFA& nfa;
    map<int, set<int>> epsilonClosures;
};
