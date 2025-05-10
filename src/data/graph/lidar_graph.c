#include <Python.h>
#include <stdlib.h>
#include <math.h>

#define DEG2RAD (M_PI / 180.0)
#define MAX_EDGES_PER_NODE 5

typedef struct {
    double x;
    double y;
} Node;

typedef struct {
    int from;
    int to;
    double weight;
} Edge;

Node* nodes;
Edge* edges;
int edge_count = 0;
int num_nodes = 0;

/* 初期化関数 */
static PyObject* initialize(PyObject* self, PyObject* args) {
    if (!PyArg_ParseTuple(args, "i", &num_nodes)) {
        return NULL;
    }
    edge_count = 0;

    // メモリ確保
    nodes = (Node*)malloc(num_nodes * sizeof(Node));
    edges = (Edge*)malloc((num_nodes * MAX_EDGES_PER_NODE) * sizeof(Edge));

    Py_RETURN_NONE;
}

/* ノードの座標をPythonに返す */
static PyObject* get_node_positions(PyObject* self, PyObject* args) {
    PyObject* positions_list = PyList_New(num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        PyObject* coords = PyTuple_New(2);
        PyTuple_SetItem(coords, 0, PyFloat_FromDouble(nodes[i].x));
        PyTuple_SetItem(coords, 1, PyFloat_FromDouble(nodes[i].y));
        PyList_SetItem(positions_list, i, coords);
    }
    return positions_list;
}


/* グラフ構築関数 */
static PyObject* build_graph(PyObject* self, PyObject* args) {
    PyObject* input_list;

    if (!PyArg_ParseTuple(args, "O", &input_list)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list as input");
        return NULL;
    }

    if (!PyList_Check(input_list)) {
        PyErr_SetString(PyExc_TypeError, "Input is not a list");
        return NULL;
    }

    int list_size = PyList_Size(input_list);
    if (list_size < 2) {
        PyErr_SetString(PyExc_ValueError, "List must have at least two elements");
        return NULL;
    }

    double angle_increment = 270.0 / list_size;

    // ノードの座標計算
    for (int i = 0; i < list_size; i++) {
        double distance = PyFloat_AsDouble(PyList_GetItem(input_list, i));
        double angle = (-135.0 + i * angle_increment) * DEG2RAD;
        nodes[i].x = distance * cos(angle);
        nodes[i].y = distance * sin(angle);
    }

    // エッジリストの構築
    edge_count = 0;
    for (int i = 0; i < list_size - 1; i++) {
        double dx = nodes[i + 1].x - nodes[i].x;
        double dy = nodes[i + 1].y - nodes[i].y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < 1.0) { // 1.0以内ならエッジを張る
            edges[edge_count].from = i;
            edges[edge_count].to = i + 1;
            edges[edge_count].weight = dist;
            edge_count++;
        }
    }

    // Pythonリストへ変換
    PyObject* edge_list = PyList_New(edge_count);
    for (int i = 0; i < edge_count; i++) {
        PyObject* edge_tuple = PyTuple_New(3);
        PyTuple_SetItem(edge_tuple, 0, PyLong_FromLong(edges[i].from));
        PyTuple_SetItem(edge_tuple, 1, PyLong_FromLong(edges[i].to));
        PyTuple_SetItem(edge_tuple, 2, PyFloat_FromDouble(edges[i].weight));
        PyList_SetItem(edge_list, i, edge_tuple);
    }

    return edge_list;
}

static PyMethodDef LidarGraphMethods[] = {
    {"initialize", initialize, METH_VARARGS, "Initialize the graph size."},
    {"build_graph", build_graph, METH_VARARGS, "Build graph from LiDAR data."},
    {"get_node_positions", get_node_positions, METH_NOARGS, "Get node positions."},
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
