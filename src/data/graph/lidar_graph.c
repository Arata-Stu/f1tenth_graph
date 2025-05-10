#include <Python.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int num_nodes;
    int **adj_matrix;
} Graph;

// 初期化
static Graph* initialize(int num_nodes) {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->num_nodes = num_nodes;
    graph->adj_matrix = (int **)malloc(num_nodes * sizeof(int *));
    for (int i = 0; i < num_nodes; i++) {
        graph->adj_matrix[i] = (int *)calloc(num_nodes, sizeof(int));
    }
    return graph;
}

// エッジの追加
static void add_edge(Graph *graph, int src, int dest) {
    if (src < graph->num_nodes && dest < graph->num_nodes) {
        graph->adj_matrix[src][dest] = 1;
        graph->adj_matrix[dest][src] = 1; // 無向グラフとして処理
    }
}

// エッジの削除
static void remove_edge(Graph *graph, int src, int dest) {
    if (src < graph->num_nodes && dest < graph->num_nodes) {
        graph->adj_matrix[src][dest] = 0;
        graph->adj_matrix[dest][src] = 0;
    }
}

// 隣接ノードの取得
static PyObject* get_neighbors(Graph *graph, int node) {
    PyObject *list = PyList_New(0);
    for (int i = 0; i < graph->num_nodes; i++) {
        if (graph->adj_matrix[node][i] == 1) {
            PyList_Append(list, PyLong_FromLong(i));
        }
    }
    return list;
}

// LiDARデータからグラフ構築
static PyObject* build_graph_from_lidar(PyObject *self, PyObject *args) {
    PyObject *capsule;
    PyObject *lidar_data;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &lidar_data)) {
        return NULL;
    }
    Graph *graph = (Graph *)PyCapsule_GetPointer(capsule, "Graph");

    int len = PyList_Size(lidar_data);
    for (int i = 0; i < len - 1; i++) {
        add_edge(graph, i, i + 1);
    }
    // 最後の点と最初の点を接続
    add_edge(graph, len - 1, 0);

    Py_RETURN_NONE;
}

// Pythonに公開するメソッド
static PyObject* py_initialize(PyObject *self, PyObject *args) {
    int num_nodes;
    if (!PyArg_ParseTuple(args, "i", &num_nodes)) {
        return NULL;
    }
    Graph *graph = initialize(num_nodes);
    return PyCapsule_New(graph, "Graph", NULL);
}

static PyObject* py_add_edge(PyObject *self, PyObject *args) {
    PyObject *capsule;
    int src, dest;
    if (!PyArg_ParseTuple(args, "Oii", &capsule, &src, &dest)) {
        return NULL;
    }
    Graph *graph = (Graph *)PyCapsule_GetPointer(capsule, "Graph");
    add_edge(graph, src, dest);
    Py_RETURN_NONE;
}

static PyObject* py_get_neighbors(PyObject *self, PyObject *args) {
    PyObject *capsule;
    int node;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &node)) {
        return NULL;
    }
    Graph *graph = (Graph *)PyCapsule_GetPointer(capsule, "Graph");
    return get_neighbors(graph, node);
}

static PyObject* py_build_graph(PyObject *self, PyObject *args) {
    return build_graph_from_lidar(self, args);
}

static PyMethodDef LidarGraphMethods[] = {
    {"initialize", py_initialize, METH_VARARGS, "Initialize the graph"},
    {"add_edge", py_add_edge, METH_VARARGS, "Add an edge to the graph"},
    {"get_neighbors", py_get_neighbors, METH_VARARGS, "Get neighbors of a node"},
    {"build_graph", py_build_graph, METH_VARARGS, "Build the graph from LiDAR data"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef lidargraphmodule = {
    PyModuleDef_HEAD_INIT,
    "lidar_graph",
    NULL,
    -1,
    LidarGraphMethods
};

PyMODINIT_FUNC PyInit_lidar_graph(void) {
    return PyModule_Create(&lidargraphmodule);
}