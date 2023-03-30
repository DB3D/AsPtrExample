
// EXAMPLE 1

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>


/////////////////////////////////////////////////////
// Include external dependencies to cast `Object` struct
//////////////////////////////////////////////////////

#include "DNA_object_types.h" //found in: ../3.4/source/blender/makesdna
#include "BKE_main.h"         //found in: ../3.4/source/blender/blenkernel
#include "BKE_object.h"       //found in: ../3.4/source/blender/blenkernel
#include "BLI_utildefines.h"  //found in: ../3.4/source/blender/blenlib

                              //Make sure your IDE external dependences can access those!
                              //Note that for some struct might be much more trickier to include! Mesh struct for example..

//////////////////////////////////////////////////////
// Define pyd function 
//////////////////////////////////////////////////////


static PyObject* read_obj_data(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }
    
    std::cout << "running read_obj_data in C++" << std::endl;

    uintptr_t address = static_cast<uintptr_t>(address_int);
    Object* obj = reinterpret_cast<Object*>(address);

    std::string obj_name(obj->id.name + 2);

    return Py_BuildValue("s", obj_name.c_str());
}