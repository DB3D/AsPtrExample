
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

/* Mesh Vertices. Typically accessed from #Mesh.verts()*/
typedef struct MVert {
  float co[3];
  /* Deprecated flag for storing hide status and selection, which are now stored in separate generic attributes. Kept for file read and write. */
  char flag_legacy;
  /* Deprecated bevel weight storage, now located in #CD_BWEIGHT, except for file read and write. */
  char bweight_legacy;
  char _pad[2];
} MVert;

/* Mesh Edges. Typically accessed with #Mesh.edges() */
typedef struct MEdge {
  /* Un-ordered vertex indices (cannot match). */
  unsigned int v1, v2;
  /* Deprecated edge crease, now located in #CD_CREASE, except for file read and write. */
  char crease_legacy;
  /* Deprecated bevel weight storage, now located in #CD_BWEIGHT, except for file read and write.*/
  char bweight_legacy;
  short flag;
} MEdge;

/* Mesh Faces.This only stores the polygon size & flags, the vertex & edge indices are stored in the #MLoop. Typically accessed with #Mesh.polys(). */
typedef struct MPoly {
  /* Offset into loop array and number of loops in the face. */
  int loopstart;
  /* Keep signed since we need to subtract when getting the previous loop. */
  int totloop;
  /* Deprecated material index. Now stored in the "material_index" attribute, but kept for IO. */
  short mat_nr_legacy;
  char flag, _pad;
} MPoly;

/* Mesh Face Corners. "Loop" is an internal name for the corner of a polygon (#MPoly). Typically accessed with #Mesh.loops(). */
typedef struct MLoop {
  /* Vertex index into an #MVert array. */
  unsigned int v;
  /* Edge index into an #MEdge array. */
  unsigned int e;
} MLoop;

} //namespace mirr

// Main function:

static PyObject* read_mesh_data_mirror(PyObject* self, PyObject* args) {
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

    std::cout << "running read_mesh_data_mirror in C++" << std::endl;

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
