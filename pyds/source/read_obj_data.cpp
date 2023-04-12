/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example1: Access obj data directly by reading Object struct

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

#include "DNA_object_types.h" // Access: struct Object
#include "DNA_object_enums.h" // Access: eDrawType

// Main function:

static PyObject* read_obj_data(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    // check if args are correct
    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    };
    
    std::cout << "running read_obj_data in C++" << std::endl;

    // interpret memory
    uintptr_t address = static_cast<uintptr_t>(address_int);
    Object* obj = reinterpret_cast<Object*>(address);

    // example on reading simple properties
    std::cout << " obj->id.name : " << obj->id.name << std::endl;
    std::cout << " obj->loc[]: (" << obj->loc[0] << ", " << obj->loc[1] << ", " << obj->loc[2] << ")" << std::endl;
    std::cout << " obj->scale[]: (" << obj->scale[0] << ", " << obj->scale[1] << ", " << obj->scale[2] << ")" << std::endl;
    
    // example on reading an enum property
    std::string draw_type_str;
    switch (obj->dt) {
        case OB_BOUNDBOX: draw_type_str = "BOUNDS"; break;
        case OB_WIRE: draw_type_str = "WIRE"; break;
        case OB_SOLID: draw_type_str = "SOLID"; break;
        case OB_MATERIAL: draw_type_str = "MATERIAL"; break;
        case OB_TEXTURE: draw_type_str = "TEXTURED"; break;
        case OB_RENDER: draw_type_str = "RENDERED"; break;
        default: draw_type_str = "UNKNOWN"; break;
    };
    std::cout << " obj->dt : " << draw_type_str << std::endl;

    //return None, this function just print in console
    Py_RETURN_NONE;
};