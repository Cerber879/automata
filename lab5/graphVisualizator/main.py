import argparse
import networkx as nx
from pyvis.network import Network

def read_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Удаляем пустые строки и разделяем строки по табуляции
    table = [line.strip().replace(' ', '').replace('\t', '') for line in lines if line.strip()]
    out_symbols = table[0][1:].split(';')
    states = table[1][1:].split(';')
    transitions = table[2:]

    return [out_symbols, states, transitions]

def create_graph(transitions):
    G = nx.DiGraph()

    header = {}

    # Добавляем узлы
    for i in range(len(transitions[1])):
        transition = transitions[1][i] + '/' + transitions[0][i]
        header[transitions[1][i]] = transition
        G.add_node(transition)

    # Добавляем ребра
    for row in transitions[2]:
        line = row.split(';')
        symbol = line[0]
        parts = line[1:]

        for index, state in enumerate(transitions[1]):
            transition = parts[index]
            if transition != '-':
                transition = header[parts[index]]
                if G.has_edge(header[state], transition):
                    # Если ребро уже существует, обновляем метку
                    current_label = G.edges[header[state], transition]['label']
                    new_label = current_label + "," + symbol
                    G.edges[header[state], transition]['label'] = new_label
                else:
                    # Если ребра нет, добавляем его с меткой
                    G.add_edge(header[state], transition, label=symbol)

    return G

def visualize_graph(G):
    net = Network(notebook=True, directed=True)

    # Добавляем узлы и ребра в pyvis
    for node in G.nodes():
        net.add_node(node)

    for edge in G.edges(data=True):
        src = edge[0]
        dst = edge[1]
        label = edge[2].get('label', '')
        net.add_edge(src, dst, arrows='to', label=label)

    net.show('graph.html')

def main(file_path):
    table_moore = read_file(file_path)
    G = create_graph(table_moore)
    visualize_graph(G)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process a file to create and visualize a graph.")
    parser.add_argument("file_path", type=str, help="Path to the input file")
    args = parser.parse_args()
    main(args.file_path)
    # main("test.txt")
