from collections import defaultdict, deque

void_transition = '@'
end_state_name = -1

input_file = "C:/Users/User/source/repos/automata/lab5python/input.txt"
output_file = "C:/Users/User/source/repos/automata/lab5python/output.txt"

current_state_name = 0

# CONVERT TO NFA


def read_regex_from_file(filename):
    with open(filename, 'r') as file:
        regex = ''.join([line.strip() for line in file if line.strip()])
    print(regex)
    return regex


def make_transition(count, states, start_id, last_symbol, current_symbol):
    global current_state_name
    if count == 1:
        states[start_id][last_symbol].add(current_state_name + 1)
        print(f"{start_id} -- {last_symbol} --> {current_state_name + 1}")
        states[current_state_name + 1][current_symbol].add(current_state_name + 1)
        print(f"{current_state_name + 1} -- {current_symbol} --> {current_state_name + 1}")
    else:
        states[start_id][current_symbol].add(current_state_name + 1)
        print(f"{start_id} -- {current_symbol} --> {current_state_name + 1}")


def process_operator(symbol_queue, states, last_symbol, need_beginning, start_id, is_last_char_operator):
    count = len(symbol_queue) if is_last_char_operator else 0
    global current_state_name
    while symbol_queue:
        current_symbol = symbol_queue.popleft()
        if need_beginning:
            make_transition(count, states, start_id, last_symbol, current_symbol)
            need_beginning = False
        else:
            make_transition(count, states, current_state_name, last_symbol, current_symbol)
        current_state_name += 1
        count -= 1


def build_end_transitions(end_states, states, end_state):
    for state_id in end_states:
        states[state_id][void_transition].add(end_state)
        print(f"{state_id} -- {void_transition} --> {end_state}")
    end_states.clear()


def regular_expression_to_nfa(regex):
    states = defaultdict(lambda: defaultdict(set))
    ordered_symbols = set()
    global current_state_name

    need_beginning = False
    start_state = [current_state_name]
    end_states_stack = [[]]
    symbol_queue = deque()

    i = 0
    while i < len(regex):
        if regex[i] == void_transition:
            symbol_queue.append(regex[i])
        elif regex[i].isalnum():
            ordered_symbols.add(regex[i])
            symbol_queue.append(regex[i])
        elif regex[i] == '*':
            process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], True)
            if need_beginning:
                need_beginning = False
        elif regex[i] == '+':
            process_operator(symbol_queue, states, symbol_queue[-1], need_beginning, start_state[-1], True)
            if need_beginning:
                need_beginning = False
        elif regex[i] == '|':
            process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], False)
            end_states_stack[-1].append(current_state_name)
            need_beginning = True
        elif regex[i] == '(':
            end_states_stack.append([])
            process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], False)
            symbol_queue.append(void_transition)
            process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], False)
            start_state.append(current_state_name)
        elif regex[i] == ')':
            process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], False)
            need_beginning = False
            end_states_stack[-1].append(current_state_name)

            if i + 1 < len(regex) and regex[i + 1] == '*':
                build_end_transitions(end_states_stack[-1], states, start_state[-1])
                states[start_state[-1]][void_transition].add(current_state_name + 1)
                print(f"{start_state[-1]} -- {void_transition} --> {current_state_name + 1}")
                current_state_name += 1
                i += 1
            elif i + 1 < len(regex) and regex[i + 1] == '+':
                build_end_transitions(end_states_stack[-1], states, current_state_name + 1)
                states[current_state_name][void_transition].add(start_state[-1])
                print(f"{current_state_name} -- {void_transition} --> {start_state[-1]}")
                current_state_name += 1
                i += 1
            else:
                build_end_transitions(end_states_stack[-1], states, current_state_name + 1)
                current_state_name += 1

            start_state.pop()
            end_states_stack.pop()

        i += 1

    process_operator(symbol_queue, states, void_transition, need_beginning, start_state[-1], False)
    end_states_stack[-1].append(current_state_name)
    build_end_transitions(end_states_stack[-1], states, end_state_name)

    return [states, ordered_symbols, current_state_name + 1]


# EPSILON CLOSURES


def compute(count_states, states):
    epsilon_closures = defaultdict(set)

    for state in range(count_states):
        compute_closure(states, epsilon_closures, state)
    return epsilon_closures


def print_closures(epsilon_closures):
    for state, closure in epsilon_closures.items():
        print(f"{state}: ", end="")
        print(", ".join(map(str, closure)))


def compute_closure(table, epsilon_closures, state):
    stack = deque([state])
    closure = set([state])

    while stack:
        curr = stack.pop()

        if '@' in table[curr]:
            for next_state in table[curr]['@']:
                if next_state not in closure:
                    closure.add(next_state)
                    stack.append(next_state)

    epsilon_closures[state] = closure


# CONVERT TO DFA


def add_adjustment_state(count_iteration, current_name_state, adjustment_states):
    adjustment_name_state = f"{'q'}{count_iteration}"

    for child_name_state in current_name_state:
        if child_name_state == end_state_name:
            adjustment_name_state += "(end)"
            break

    adjustment_states[tuple(current_name_state)] = adjustment_name_state


def nfa_to_dfa(ordered_symbols, nfa_table, epsilon_closures):
    dfa_table = defaultdict(lambda: defaultdict(str))
    dfa_states = set()
    ordered_states = []
    state_queue = deque()

    start_state = list(epsilon_closures[0])
    state_queue.append(start_state)
    dfa_states.add(tuple(start_state))

    adjustment_states = {}
    add_adjustment_state(0, start_state, adjustment_states)
    ordered_states.append(adjustment_states[tuple(start_state)])

    count_iteration = 1

    while state_queue:
        current_name_state = state_queue.popleft()

        for symbol in ordered_symbols:
            unique_states = set()
            for child_name_state in current_name_state:
                if child_name_state != -1 and symbol in nfa_table[child_name_state]:
                    for next_state in nfa_table[child_name_state][symbol]:
                        unique_states.update(epsilon_closures[next_state])

            new_name_state = sorted(list(unique_states))

            if not new_name_state:
                dfa_table[adjustment_states[tuple(current_name_state)]][symbol] = "-"
                print(f"{adjustment_states[tuple(current_name_state)]} -- {symbol} --> -")
            else:
                if tuple(new_name_state) not in dfa_states:
                    add_adjustment_state(count_iteration, new_name_state, adjustment_states)
                    count_iteration += 1

                    state_queue.append(new_name_state)
                    dfa_states.add(tuple(new_name_state))
                    ordered_states.append(adjustment_states[tuple(new_name_state)])

                dfa_table[adjustment_states[tuple(current_name_state)]][symbol] = adjustment_states[tuple(new_name_state)]
                print(f"{adjustment_states[tuple(current_name_state)]} -- {symbol} --> {adjustment_states[tuple(new_name_state)]}")

    return [dfa_table, ordered_states]


def write_to_file_dfa(filename, symbols, dfa_table, ordered_states):
    with open(filename, "w") as file:
        file.write(";" + ";".join(ordered_states) + "\n")

        for symbol in symbols:
            file.write(f"{symbol}")
            for state in ordered_states:
                file.write(f";{dfa_table[state][symbol]}")
            file.write("\n")


def main():
    regular_expression = read_regex_from_file(input_file)
    nfa_data = regular_expression_to_nfa(regular_expression)

    symbols = nfa_data[1]
    ordered_symbols = sorted(symbols)
    nfa_table = nfa_data[0]
    quantity_states = nfa_data[2]

    epsilon_closures = compute(quantity_states, nfa_table)
    print_closures(epsilon_closures)

    dfa_data = nfa_to_dfa(ordered_symbols, nfa_table, epsilon_closures)

    dfa_table = dfa_data[0]
    ordered_states = dfa_data[1]

    write_to_file_dfa(output_file, ordered_symbols, dfa_table, ordered_states)


if __name__ == "__main__":
    main()