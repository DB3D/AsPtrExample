/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example7: Write object mesh data from memory, offset vertices on Z axis by given value

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

#include "DNA_object_types.h" // Access struct Object
#include "DNA_mesh_types.h" // Access struct Mesh
#include "BKE_mesh.h" // Access CustomData_get_layer

// Unused
// #include "BKE_context.h" // Access bContext
// #include "DEG_depsgraph.h" // Access DEG_id_tag_update
// #include "WM_api.h" // send update signal

// Main function:

static PyObject* write_mesh_data(PyObject* self, PyObject* args) {
    unsigned long long address_int;
    float zpush;

    // Check if args are correct
    if (!PyArg_ParseTuple(args, "Kf", &address_int, &zpush)) {
        return nullptr;
    };

    std::cout << "running write_mesh_data in C++" << std::endl;

    // Unusured, I tried hard to work with bContext in this example by always failed. For some reasons, bContext always seems empty
    //
    // interpret memory of our context address
    // uintptr_t adress = static_cast<uintptr_t>(address_int);
    // bContext* C = reinterpret_cast<bContext*>(adress);
    // if (C == nullptr) {
    //     PyErr_SetString(PyExc_RuntimeError, "Invalid context pointer");
    //     return nullptr;
    // }
    // // get active object from context
    // Object* obj = CTX_data_active_object(C);
    // if (obj == nullptr) {
    //     PyErr_SetString(PyExc_RuntimeError, "No active object or no object data");
    //     return nullptr;
    // }

    // interpret memory
    uintptr_t address = static_cast<uintptr_t>(address_int);
    Object* obj = reinterpret_cast<Object*>(address);
    Mesh* mesh = static_cast<Mesh*>(obj->data);

    // example reading first 3 vertices co
    MVert *vdata = static_cast<MVert *>(CustomData_get_layer(&mesh->vdata, CD_MVERT));
    if (vdata != nullptr) {
        for (int i = 0; i < mesh->totvert; ++i) {
            vdata[i].co[2] = vdata[i].co[2] + zpush;
        };
    };

    // mark current cache as dirty
    BKE_mesh_batch_cache_dirty_tag(static_cast<Mesh *>(obj->data), BKE_MESH_BATCH_DIRTY_ALL);

    //return None, this function just print in console
    Py_RETURN_NONE;
};