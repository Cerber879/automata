import csv
import matplotlib.pyplot as plt
import networkx as nx

input_file_mealy = 'input_mealy.csv'
input_file_moore = 'input_moore.csv'
output_file = 'output.csv'
status = 'moore'
quantity_rows_header = 2 if status == 'mealy' else 1


# FUNCTIONS FOR MEALY -> MOORE


def conditions_automata(symbol_condition, quantity_conditions):
    conditions = []
    for i in range(quantity_conditions):
        conditions.append(symbol_condition + str(i))

    return conditions


def selection_unique_transitions(lines):
    transitions = []
    conditions = conditions_automata(lines[0][0][:1], len(lines[0]))

    for sublist in lines[1:]:
        for item in sublist[1:]:
            if item not in transitions:
                transitions.append(item)
                if conditions and item.split('/')[0] in conditions:
                    conditions.remove(item.split('/')[0])

    for condition in conditions:
        transitions.append(condition)

    return transitions


def filling_header_moore_table(transitions):
    table_moore = []
    conditions = []
    output_characters = []

    ind = 0

    for transit in transitions:
        conditions.append('q' + str(ind))

        if '/' in transit:
            temp = ''.join(transit).replace('/', ' ').split()
            output_ch = temp[1]
            output_characters.append(output_ch)
        else:
            output_characters.append('-')

        ind += 1

    table_moore.append(output_characters)
    table_moore.append(conditions)

    return table_moore


def filling_transitions_moore_table(num_input_ch, transitions, table_mealy):
    transitions_input_ch = ['x' + str(num_input_ch)]
    prev_number_s = ''

    for transit in transitions:
        number_s = transit.split('/')[0][1:]
        if number_s == prev_number_s:
            transitions_input_ch.append(transitions_input_ch[-1])
        else:
            index_condition = transitions.index(table_mealy[num_input_ch][int(number_s) + 1])
            transitions_input_ch.append('q' + str(index_condition))

    return transitions_input_ch


# FUNCTIONS FOR MEALY


# FUNCTIONS FOR MOORE -> MEALY


def filling_transitions_mealy_table(num_row, table_moore):
    transitions_input_ch = ['x' + str(num_row - 1)]

    for num_transit in range(1, len(table_moore[num_row])):
        try:
            number_condition = int(table_moore[num_row][num_transit][0:])
        except:
            number_condition = int(table_moore[num_row][num_transit][1:])

        transitions_input_ch.append(table_moore[1][number_condition] + '/' + table_moore[0][number_condition])

    return transitions_input_ch


# FUNCTIONS FOR MOORE -> MEALY


def read_rows_csvfile(row):
    lst = []
    for i in row:
        lst.append(i)

    return lst


def print_output(file, table):
    with open(file, 'w') as csvfile:
        cnt = 0
        for row in table:
            if cnt >= quantity_rows_header:
                csvfile.write(';'.join(row) + '\n')
            else:
                csvfile.write(';' + ';'.join(row) + '\n')
            cnt += 1


def construction_graph_moore(table):
    Graph = nx.DiGraph()

    for num_row in range(2, len(table)):
        for num_column in range(1, len(table[num_row])):
            q1 = table[1][num_column - 1] + '/' + table[0][num_column - 1]
            q2 = table[num_row][num_column] + '/' + table[0][int(table[num_row][num_column][1:])]
            Graph.add_edge(q1, q2, label=table[num_row][0])

    return Graph


def construction_graph_mealy(table):
    Graph = nx.DiGraph()

    for num_row in range(1, len(table)):
        for num_column in range(1, len(table[num_row])):
            transition = table[num_row][num_column].split('/')
            q1 = table[0][num_column - 1]
            q2 = transition[0]
            w = table[num_row][0] + '/' + transition[1]
            Graph.add_edge(q1, q2, label=w)

    return Graph


def draw_graph(table, graph_construction):
    G = graph_construction(table)

    # Определение позиций узлов на графе
    pos = nx.spring_layout(G)

    # Рисование графа с помощью matplotlib
    nx.draw_networkx(G, pos, with_labels=True, node_size=800, node_color='lightblue', font_size=10)

    grafo_labels = nx.get_edge_attributes(G, 'label')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=grafo_labels)

    # Отображение графика
    plt.axis('off')
    plt.show()


if status == 'mealy':
    with open(input_file_mealy, 'r') as csvfile:
        reader = csv.reader(csvfile)

        table_mealy = []

        for row in reader:
            new_row = ''.join(row).replace(';', ' ').strip().split()
            table_mealy.append(read_rows_csvfile(new_row))

    transitions = selection_unique_transitions(table_mealy)
    transitions.sort()

    table_moore = filling_header_moore_table(transitions)

    for input_ch in range(1, len(table_mealy)):
        transitions_input_ch = filling_transitions_moore_table(input_ch, transitions, table_mealy)
        table_moore.append(transitions_input_ch)

    print_output(output_file, table_moore)
    draw_graph(table_moore, construction_graph_moore)


elif status == 'moore':
    with open(input_file_moore, 'r') as csvfile:
        reader = csv.reader(csvfile)

        table_moore = []

        for row in reader:
            new_row = ''.join(row).replace(';', ' ').strip().split()
            table_moore.append(read_rows_csvfile(new_row))

        table_mealy = [table_moore[1]]

        for input_ch in range(2, len(table_moore)):
            transitions_input_ch = filling_transitions_mealy_table(input_ch, table_moore)
            table_mealy.append(transitions_input_ch)

        print_output(output_file, table_mealy)
        draw_graph(table_mealy, construction_graph_mealy)
