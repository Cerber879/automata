import csv
import copy
import matplotlib.pyplot as plt
import networkx as nx

input_file_mealy = 'input_mealy.csv'
input_file_moore = 'input_moore.csv'
output_file = 'output.csv'
status = 'mealy'

is_mealy = True if status == 'mealy' else False
start_rows_transitions = 1 if status == 'mealy' else 2
quantity_rows_header = start_rows_transitions


def minimization_conditions(table, first_passage, prev_conditions):
    unique_output_ch_condition = []
    conditions = []
    for num_column in range(1, len(table[start_rows_transitions])):
        output_ch_condition = ''

        for num_row in range(start_rows_transitions, len(table)):
            if first_passage:
                if is_mealy:
                    output_ch_condition += table[num_row][num_column].split('/')[1]
                else:
                    output_ch_condition = table[0][num_column - 1]
                    break
            else:
                output_ch_condition += str(table[num_row][num_column])

        if prev_conditions:
            output_ch_condition += str(prev_conditions[num_column - 1])

        if output_ch_condition not in unique_output_ch_condition:
            unique_output_ch_condition.append(output_ch_condition)
            conditions.append(len(unique_output_ch_condition) - 1)
        else:
            ind = unique_output_ch_condition.index(output_ch_condition)
            conditions.append(ind)

    return conditions


def completion_new_table(table, conditions):
    copy_table = copy.deepcopy(table)
    for num_column in range(1, len(table[start_rows_transitions])):
        for num_row in range(start_rows_transitions, len(table)):
            if is_mealy:
                condition_of_transit = table[num_row][num_column].split('/')[0]
            else:
                condition_of_transit = table[num_row][num_column]

            copy_table[num_row][num_column] = conditions[int(condition_of_transit) - offset_numeration]

    return copy_table


def minimization(original_table):
    conditions = minimization_conditions(original_table, True, [])
    print(original_table)
    prev_conditions = copy.deepcopy(conditions)
    while True:
        new_table = completion_new_table(original_table, conditions)
        conditions = minimization_conditions(new_table, False, prev_conditions)
        if conditions == prev_conditions:
            return [new_table, conditions]

        prev_conditions = copy.deepcopy(conditions)


def delete_excess_conditions(general_table, count_delete_conditions):
    if count_delete_conditions > 0:
        for row in general_table:
            del row[-count_delete_conditions:]


def general_view(original_table, table, conditions):
    passed_conditions = []
    count_delete_conditions = 0
    for num_column in range(1, len(table[start_rows_transitions])):
        if conditions[num_column - 1] in passed_conditions:
            count_delete_conditions += 1
            continue

        passed_conditions.append(conditions[num_column - 1])

        for num_row in range(start_rows_transitions, len(table)):
            if is_mealy:
                output_ch = original_table[num_row][num_column].split('/')[1]
                condition = table[num_row][num_column]
                table[num_row][num_column - count_delete_conditions] = 's' + str(condition) + '/y' + output_ch
                table[0][num_column - count_delete_conditions - 1] = 's' + str(passed_conditions[-1])
            else:
                condition = table[num_row][num_column]
                table[num_row][num_column - count_delete_conditions] = 's' + str(condition)
                table[1][num_column - count_delete_conditions - 1] = 's' + str(passed_conditions[-1])
                table[0][num_column - count_delete_conditions - 1] = 'y' + original_table[0][num_column - 1]

    delete_excess_conditions(table, count_delete_conditions)


def read_rows_csvfile(row):
    lst = []
    for i in row:
        if any(char.isalpha() for char in i):
            if '/' in i:
                temp = i.split('/')
                first_dict = ''.join(filter(str.isdigit, temp[0]))
                second_dict = ''.join(filter(str.isdigit, temp[1]))
                i = first_dict + '/' + second_dict
            else:
                i = ''.join(filter(str.isdigit, i))

        lst.append(i)

    return lst


def print_output(file, table):
    with open(file, 'w') as csvfile:
        cnt = 0
        for row in table:
            if cnt >= quantity_rows_header:
                csvfile.write('x' + ';'.join(row) + '\n')
            else:
                csvfile.write(';' + ';'.join(row) + '\n')
            cnt += 1


def construction_graph_moore(table):
    Graph = nx.DiGraph()

    for num_row in range(2, len(table)):
        for num_column in range(1, len(table[num_row])):
            q1 = table[1][num_column - 1] + '/' + table[0][num_column - 1]
            q2 = table[num_row][num_column] + '/' + table[0][int(table[num_row][num_column][1:])]
            x = 'x' + table[num_row][0]
            Graph.add_edge(q1, q2, label=x)

    return Graph


def construction_graph_mealy(table):
    Graph = nx.DiGraph()

    for num_row in range(1, len(table)):
        for num_column in range(1, len(table[num_row])):
            transition = table[num_row][num_column].split('/')
            q1 = table[0][num_column - 1]
            q2 = transition[0]
            w = 'x' + table[num_row][0] + '/' + transition[1]
            Graph.add_edge(q1, q2, label=w)

    return Graph


def draw_graph(table, graph_construction):
    G = graph_construction(table)

    # Определение позиций узлов на графе
    pos = nx.spring_layout(G)

    # Рисование графа с помощью matplotlib
    nx.draw_networkx(G, pos, with_labels=True, node_size=400, node_color='lightblue', font_size=10)

    grafo_labels = nx.get_edge_attributes(G, 'label')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=grafo_labels)

    # Отображение графика
    plt.axis('off')
    plt.show()


if status == 'mealy':

    table_mealy = []
    with open(input_file_mealy, 'r') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            new_row = ''.join(row).replace(';', ' ').strip().split()
            table_mealy.append(read_rows_csvfile(new_row))

    offset_numeration = 0 if int(table_mealy[0][0]) == 0 else 1
    print(table_mealy)

    data = minimization(table_mealy)
    minimization_table = data[0]
    conditions = data[1]

    general_view(table_mealy, minimization_table, conditions)
    print_output(output_file, minimization_table)
    draw_graph(minimization_table, construction_graph_mealy)

elif status == 'moore':

    table_moore = []
    with open(input_file_moore, 'r') as csvfile:
        reader = csv.reader(csvfile)

        for row in reader:
            new_row = ''.join(row).replace(';', ' ').strip().split()
            table_moore.append(read_rows_csvfile(new_row))

    offset_numeration = 0 if int(table_moore[1][0]) == 0 else 1

    data = minimization(table_moore)
    minimization_table = data[0]
    conditions = data[1]
    print(conditions)
    for i in minimization_table:
        print(i)

    general_view(table_moore, minimization_table, conditions)
    print_output(output_file, minimization_table)
    draw_graph(minimization_table, construction_graph_moore)
