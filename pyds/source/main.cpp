/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

#include "buildinfo.h" // buildinfo needed in order to build with included BKE files
                       // else, won't compile...
#include <Python.h>

#include "read_obj_data.cpp"             // Ex 1
#include "read_mesh_elements_mirror.cpp" // Ex 2
#include "read_mesh_data.cpp"            // Ex 3
#include "render_obj.cpp"                // Ex 4
#include "read_mesh_data_mirror.cpp"     // Ex 5
#include "write_img_data.cpp"            // Ex 6
#include "write_mesh_data.cpp"           // Ex 7
#include "write_scene_data.cpp"          // Ex 8

//Define functions
static PyMethodDef ReadMemFunctions[] = {
    {"read_obj_data", read_obj_data, METH_VARARGS, "Read Object data properties"},
    {"read_mesh_elements_mirror", read_mesh_elements_mirror, METH_VARARGS, "Read Mesh elements via mirror structs."},
    {"read_mesh_data", read_mesh_data, METH_VARARGS, "Read Mesh data properties and mesh structure"},
    {"render_obj", render_obj, METH_VARARGS, "Render the object in a quick HD wireframe render."},
    {"read_mesh_data_mirror", read_mesh_data_mirror, METH_VARARGS, "Read Mesh data via mirror structs and function."},
    {"write_img_data", write_img_data, METH_VARARGS, "Directly write an image from memory."},
    {"write_mesh_data", write_mesh_data, METH_VARARGS, "Directly write a object mesh data from memory."},
    {"write_scene_data", (PyCFunction)write_scene_data, METH_VARARGS | METH_KEYWORDS, "Directly write a scene data from memory."}, //example of named arguments

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
};