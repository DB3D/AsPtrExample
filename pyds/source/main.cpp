
#include "buildinfo.h" // buildinfo needed in order to build with included BKE files
                       // else, won't compile...

#include <Python.h>

#include "read_obj_data.cpp"
#include "read_mesh_data.cpp"
#include "read_mesh_data_mirror.cpp"
#include "render_obj.cpp"

//Define functions
static PyMethodDef ReadMemFunctions[] = {
    {"read_obj_data", read_obj_data, METH_VARARGS, "Read Object data properties"},
    {"read_mesh_data", read_mesh_data, METH_VARARGS, "Read Mesh data properties and mesh structure"},
    {"read_mesh_data_mirror", read_mesh_data_mirror, METH_VARARGS, "Read Mesh elements via mirror structs."},
    {"render_obj", render_obj, METH_VARARGS, "Render the object in a quick HD wireframe render."},
    {nullptr, nullptr, 0, nullptr}
};

//Define Modules
static struct PyModuleDef readmemmodule = {
    PyModuleDef_HEAD_INIT,
    "readmem",
    nullptr,
    -1,
    ReadMemFunctions,
};

//Create the Module
PyMODINIT_FUNC PyInit_readmem(void) {
    return PyModule_Create(&readmemmodule);
}