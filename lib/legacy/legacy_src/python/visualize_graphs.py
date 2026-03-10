import networkx as nx
import matplotlib.pyplot as plt

def visualize_graph(adjacency, highlight_nodes=None):
    """
    adjacency: dict where keys are nodes and values are lists of neighbors.
               Example: {"A": ["B", "C"], "B": ["C"], "C": []}
    highlight_nodes: list of nodes to highlight with a different color.
                     Example: ["A", "C"] or [0, 2]
    """
    G = nx.Graph()

    # Add edges based on adjacency list
    for i in range(0, len(adjacency)):
        G.add_node(i)
        for j in range(0, i):
            if adjacency[i][j] == 1:
                G.add_edge(i, j)

    # Prepare colors for nodes
    if highlight_nodes is None:
        highlight_nodes = []
    
    highlight_set = set(highlight_nodes)
    
    node_colors = ['red' if node in highlight_set else 'lightblue' 
                   for node in G.nodes()]
    
    # Prepare colors for edges
    edge_colors = []
    for u, v in G.edges():
        if u in highlight_set and v in highlight_set:
            edge_colors.append('red')
        elif u in highlight_set or v in highlight_set:
            edge_colors.append('gray')
        else:
            edge_colors.append('gray')
    
    # Draw the graph
    plt.figure(figsize=(8, 6))
    pos = nx.circular_layout(G)
    
    nx.draw(G, pos, 
            with_labels=True, 
            node_color=node_colors,
            edge_color=edge_colors,
            node_size=800, 
            font_size=12,
            width=2)
    
    plt.show()
    
def read_graph():
    with open('graph.txt', 'r', encoding='UTF-8') as f:
        return eval(f.read())
    
def read_clique():
    with open('clique.txt', 'r', encoding='UTF-8') as f:
        return eval(f.read())

graph = read_graph()
clique = read_clique()

# Ejemplo: resaltar los nodos 0, 1 y 3
visualize_graph(graph, highlight_nodes=clique)