#include <Python.h>
#include <stdlib.h>
#include <math.h>

#define DEG2RAD (M_PI / 180.0)

typedef struct {
    int from;
    int to;
    double weight;
} Edge;

Edge* edges;
int edge_count = 0;
int num_nodes = 0;

/* 初期化関数 */
static PyObject* initialize(PyObject* self, PyObject* args) {
    if (!PyArg_ParseTuple(args, "i", &num_nodes)) {
        return NULL;
    }
    edge_count = 0;  // 初期化
    edges = (Edge*)malloc((num_nodes * 5) * sizeof(Edge)); // 最大5倍のエッジ
    Py_RETURN_NONE;
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
    double* x_coords = (double*)malloc(list_size * sizeof(double));
    double* y_coords = (double*)malloc(list_size * sizeof(double));

    for (int i = 0; i < list_size; i++) {
        double distance = PyFloat_AsDouble(PyList_GetItem(input_list, i));
        double angle = (-135.0 + i * angle_increment) * DEG2RAD;
        x_coords[i] = distance * cos(angle);
        y_coords[i] = distance * sin(angle);
    }

    // エッジリストの構築
    edge_count = 0;
    for (int i = 0; i < list_size - 1; i++) {
        double dx = x_coords[i + 1] - x_coords[i];
        double dy = y_coords[i + 1] - y_coords[i];
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

    free(x_coords);
    free(y_coords);

    return edge_list;
}

static PyMethodDef LidarGraphMethods[] = {
    {"initialize", initialize, METH_VARARGS, "Initialize the graph size."},
    {"build_graph", build_graph, METH_VARARGS, "Build graph from LiDAR data."},
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
