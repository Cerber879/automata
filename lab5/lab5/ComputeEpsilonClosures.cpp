#include "ComputeEpsilonClosures.h"

map<int, set<int>> ComputeEpsilonClosures::Compute()
{
    for (int state = 0; state < nfa.countStates; ++state)
    {
        ComputeClosure(state);
    }

    return epsilonClosures;
}

void ComputeEpsilonClosures::PrintClosures() const
{
    for (const auto& [state, closure] : epsilonClosures)
    {
        cout << state << ": ";
        for (int s : closure)
        {
            cout << s << ", ";
        }
        cout << '\n';
    }
}

void ComputeEpsilonClosures::ComputeClosure(int state)
{
    stack<int> stack;
    set<int> closure;

    stack.push(state);
    closure.insert(state);

    while (!stack.empty())
    {
        int curr = stack.top();
        stack.pop();

        if (nfa.table.find(curr) != nfa.table.end() && nfa.table.at(curr).find('@') != nfa.table.at(curr).end())
        {
            for (int nextState : nfa.table.at(curr).at('@'))
            {
                if (closure.find(nextState) == closure.end())
                {
                    closure.insert(nextState);
                    stack.push(nextState);
                }
            }
        }
    }

    epsilonClosures[state] = closure;
}