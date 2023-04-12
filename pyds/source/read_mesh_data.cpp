/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example3: Access mesh data directly by reading Mesh struct

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

#include "DNA_mesh_types.h" // struct Mesh
#include "BKE_mesh.h" // CustomData_get_layer

// Note that including BKE will lead us to:
//  - set up buildinfo's 
//  - link many more libraries

// Main function:

static PyObject* read_mesh_data(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    // check if args are correct
    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    };

    std::cout << "running read_mesh_data in C++" << std::endl;

    // interpret memory
    uintptr_t address = static_cast<uintptr_t>(address_int);
    Mesh* mesh = reinterpret_cast<Mesh*>(address);

    // example on reading simple properties
    std::cout << " mesh->id.name : " << mesh->id.name << std::endl;
    std::cout << " mesh->totvert : " << mesh->totvert << std::endl;

    // example reading first 3 vertices co
    MVert *vdata = static_cast<MVert *>(CustomData_get_layer(&mesh->vdata, CD_MVERT));
    if (vdata != nullptr && mesh->totvert >= 3) {
        for (int i = 0; i < 3; ++i) {
            float x = vdata[i].co[0];
            float y = vdata[i].co[1];
            float z = vdata[i].co[2];
            std::cout << " vdata[" << i << "].co : (" << x << ", " << y << ", " << z << ")" << std::endl;
        };
    };

    //return None, this function just print in console
    Py_RETURN_NONE;
};
