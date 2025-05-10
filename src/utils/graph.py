import numpy as np
import networkx as nx
from numba import njit, prange

@njit(parallel=True)
def polar_to_cartesian(scan_data):
    angles = np.deg2rad(np.linspace(-135, 135, len(scan_data)))
    x_coords = scan_data * np.cos(angles)
    y_coords = scan_data * np.sin(angles)
    return x_coords, y_coords


@njit(parallel=True)
def build_edges(x_coords, y_coords, max_distance):
    num_points = len(x_coords)
    edges = []

    for i in prange(num_points):
        for j in range(i + 1, num_points):
            dist = np.sqrt((x_coords[i] - x_coords[j]) ** 2 + (y_coords[i] - y_coords[j]) ** 2)
            if dist < max_distance:
                edges.append((i, j))
    
    return edges


def lidar_to_graph(scan_data, max_distance=1.0):
    x_coords, y_coords = polar_to_cartesian(scan_data)
    edges = build_edges(x_coords, y_coords, max_distance)
    
    G = nx.Graph()
    for i, (x, y) in enumerate(zip(x_coords, y_coords)):
        G.add_node(i, pos=(x, y), distance=scan_data[i])
    
    # エッジの追加
    G.add_edges_from(edges)

    return G
