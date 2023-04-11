/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example2: Access mesh data by creating mirror structs and passing mesh elements array separately
//           That's your best bet, if you don't want to build your .pyd alonside blender CMake

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <array>

// replicate the mesh elements structs 
// all defined in ..\source\blender\makesdna\DNA_meshdata_types.h

namespace mirr {

//DNA_meshdata_types.h
typedef struct MVert {
  float co[3];
  char flag_legacy;
  char bweight_legacy;
  char _pad[2];
} MVert;

//DNA_meshdata_types.h
typedef struct MEdge {
  unsigned int v1, v2;
  char crease_legacy;
  char bweight_legacy;
  short flag;
} MEdge;

//DNA_meshdata_types.h
typedef struct MPoly {
  int loopstart;
  int totloop;
  short mat_nr_legacy;
  char flag, _pad;
} MPoly;

//DNA_meshdata_types.h
typedef struct MLoop {
  unsigned int v;
  unsigned int e;
} MLoop;

} //namespace mirr

// Main function:

static PyObject* read_mesh_elements_mirror(PyObject* self, PyObject* args) {
    unsigned long long vertices_address, edges_address, polygons_address, loops_address;
    int num_verts, num_edges, num_polys, num_loops;

    // check if args are correct
    if (!PyArg_ParseTuple(args, "KKKKiiii",
        &vertices_address, &edges_address, &polygons_address, &loops_address,
        &num_verts, &num_edges, &num_polys, &num_loops)) {
        return nullptr;
    }

    // interpret memory
    mirr::MVert* vertices = reinterpret_cast<mirr::MVert*>(static_cast<uintptr_t>(vertices_address));
    mirr::MEdge* edges = reinterpret_cast<mirr::MEdge*>(static_cast<uintptr_t>(edges_address));
    mirr::MPoly* polygons = reinterpret_cast<mirr::MPoly*>(static_cast<uintptr_t>(polygons_address));
    mirr::MLoop* loops = reinterpret_cast<mirr::MLoop*>(static_cast<uintptr_t>(loops_address));

    std::cout << "running read_mesh_elements_mirror in C++" << std::endl;

    // example reading first 3 vertices co
    for (int i = 0; i < 3; ++i) {
        mirr::MVert& vert = vertices[i];
        float x = vert.co[0];
        float y = vert.co[1];
        float z = vert.co[2];
        std::cout << " MVert[" << i << "].co : (" << x << ", " << y << ", " << z << ")" << std::endl;
    };

    //return None, this function just print in console
    Py_RETURN_NONE;
};
