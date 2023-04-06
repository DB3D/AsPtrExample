
#include <Python.h>

#include <string>
#include <sstream>
#include <iostream>

//////////////////////////////////////////////////////////
// Mirrorring blender struct: Two methods found:
//////////////////////////////////////////////////////////

/*
We are relying on .pyd to read blender memory, see "_FailedAttempt" folder
If you never built your own .pyd before, start with a simpler project, for example:
https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp
note that .pyd compiling will depend on OS

In order to read in blender memory, we need to be able tu use blender internal struct that can be found in the C API!
You will need:
    - search in blender code base, and find where these structs are defined
    - make sure that the memory adress you recieved correspond to the python struct you are calling as_pointer(), no doc, guessing is your best bet 
    - make sure the struct you are using are from the same version of the blender version of your plugin, blender source code change quite often
*/

//////////////////////////////////////////////////////////
// Register .pyd files
//////////////////////////////////////////////////////////

//get pyd functions 
#include "_read_obj_data.cpp"      //Example1: Reading the name from Object struct. We obtained the struct by including the original source code in our IDE
#include "_read_mesh_elements.cpp" //Example2: Reading Mesh.vertices/edges/polygons/loops by looping their arrays, we replicate the simple meshdata struct in the project
#include "_read_mesh_data.cpp"     //Example3: Reading Mesh struct directly, the Mesh struct is harder to reproduce and to link in your project. (WIP)

//Define functions
static PyMethodDef SimpleAddMethods[] = {
    {"read_obj_data",      read_obj_data,      METH_VARARGS, "Access Object data."},
    {"read_mesh_elements", read_mesh_elements, METH_VARARGS, "Access Mesh data each elements."},
    {"read_mesh_data",     read_mesh_data,     METH_VARARGS, "Access Mesh data."},
    {nullptr, nullptr, 0, nullptr}
};

//Define Modules
static struct PyModuleDef readmemmodule = {
    PyModuleDef_HEAD_INIT,
    "readmem",
    nullptr,
    -1,
    SimpleAddMethods,
};

//Create the Module
PyMODINIT_FUNC PyInit_readmem(void) {
    return PyModule_Create(&readmemmodule);
}

// Once .dll has been compiled in x64 folder, make a copy in "pydll" folder and rename it to "readmem.pyd", 
// note that __init__.py script do that automatically, for convenience