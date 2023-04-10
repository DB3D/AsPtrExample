
// Example4: Access  mesh elements from obj pointer and return a simple 1920x1080 render

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

#include "DNA_object_types.h"
#include "DNA_mesh_types.h"
#include "BKE_mesh.h"

// Describing a more simpler mesh format

struct SimpleMeshVertex {
    float x, y, z; // Vertex coordinates
};
struct SimpleMeshEdge {
    int v1, v2; // Indices of the two vertices that form the edge
};
struct SimpleMeshFace {
    std::vector<int> vertices; // Indices of the vertices that form the face
};
struct SimpleMesh {
    std::vector<SimpleMeshVertex> vertices;
    std::vector<SimpleMeshEdge> edges;
    std::vector<SimpleMeshFace> faces;
};

// Convert a blender mesh data to our custom simple mesh format

SimpleMesh convert_to_simple_mesh(Mesh* mesh) {
    SimpleMesh simple_mesh;

    // Get the mesh elements
    MVert* vertices = static_cast<MVert*>(CustomData_get_layer(&mesh->vdata, CD_MVERT));
    MEdge* edges = static_cast<MEdge*>(CustomData_get_layer(&mesh->edata, CD_MEDGE));
    MPoly* polygons = static_cast<MPoly*>(CustomData_get_layer(&mesh->pdata, CD_MPOLY));
    MLoop* loops = static_cast<MLoop*>(CustomData_get_layer(&mesh->ldata, CD_MLOOP));

    // Convert vertices
    for (int i = 0; i < mesh->totvert; ++i) {
        SimpleMeshVertex vertex;
        vertex.x = vertices[i].co[0];
        vertex.y = vertices[i].co[1];
        vertex.z = vertices[i].co[2];
        simple_mesh.vertices.push_back(vertex);
    }

    // Convert edges
    for (int i = 0; i < mesh->totedge; ++i) {
        SimpleMeshEdge edge;
        edge.v1 = edges[i].v1;
        edge.v2 = edges[i].v2;
        simple_mesh.edges.push_back(edge);
    }

    // Convert faces (polygons)
    for (int i = 0; i < mesh->totpoly; ++i) {
        SimpleMeshFace face;
        int loopstart = polygons[i].loopstart;
        int totloop = polygons[i].totloop;
        for (int j = loopstart; j < loopstart + totloop; ++j) {
            face.vertices.push_back(loops[j].v);
        }
        simple_mesh.faces.push_back(face);
    }

    return simple_mesh;
}

//Render our simple Mesh format into a HD wireframe render

std::vector<unsigned char> WireFrameRender(const SimpleMesh& mesh, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 4, 0);

    // Find the center of the mesh
    float center_x = 0, center_y = 0, center_z = 0;
    for (const auto& v : mesh.vertices) {
        center_x += v.x;
        center_y += v.y;
        center_z += v.z;
    }
    center_x /= mesh.vertices.size();
    center_y /= mesh.vertices.size();
    center_z /= mesh.vertices.size();
    // Compute the scaling factor to fit the mesh in the image
    float max_extent = 0;
    for (const auto& v : mesh.vertices) {
        float dx = v.x - center_x;
        float dy = v.y - center_y;
        float dz = v.z - center_z;
        max_extent = std::max(max_extent, std::max(std::max(std::abs(dx), std::abs(dy)), std::abs(dz)));
    }
    float scale = std::min(width, height) / (2 * max_extent);
    // Transform the vertices to screen space
    std::vector<std::pair<int, int>> vertices2d(mesh.vertices.size());
    for (int i = 0; i < mesh.vertices.size(); i++) {
        float x = mesh.vertices[i].x - center_x;
        float y = mesh.vertices[i].y - center_y;
        float z = mesh.vertices[i].z - center_z;
        vertices2d[i].first = static_cast<int>(std::round(width / 2.0f + x * scale));
        vertices2d[i].second = static_cast<int>(std::round(height / 2.0f + y * scale));
    }
    // Render the edges
    for (const auto& edge : mesh.edges) {
        std::pair<int, int> p1 = vertices2d[edge.v1];
        std::pair<int, int> p2 = vertices2d[edge.v2];
        int x1 = p1.first;
        int y1 = p1.second;
        int x2 = p2.first;
        int y2 = p2.second;
        // Bresenham's line algorithm
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;
        while (x1 != x2 || y1 != y2) {
            if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
                int index = (x1 + y1 * width) * 4;
                pixels[index] = 255;
                pixels[index + 1] = 255;
                pixels[index + 2] = 255;
                pixels[index + 3] = 255;
            }
            int err2 = 2 * err;
            if (err2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (err2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }
    return pixels;
}

// Main function:

static PyObject* render_obj(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    // check if args are correct
    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }
    
    std::cout << "running render_obj in C++" << std::endl;

    // interpret memory
    uintptr_t address = static_cast<uintptr_t>(address_int);
    Object* obj = reinterpret_cast<Object*>(address);

    // Check if object type is a mesh
    if (obj->type != OB_MESH) {
        PyErr_SetString(PyExc_TypeError, "Object is not a mesh");
        return nullptr;
    }

    // Get to the mesh data
    Mesh* mesh = static_cast<Mesh*>(obj->data);

    // Convert our arrays to a simple mesh format
    SimpleMesh simple_mesh = convert_to_simple_mesh(mesh);
    
    // Render this mesh
    int width = 1920, height = 1080;
    std::vector<unsigned char> pixels = WireFrameRender(simple_mesh, width, height);
      
    //TODO: let's pass back a numpy array instead of a bytearray.
    // Convert the pixels to a Python bytearray
    PyObject* pixel_array = PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    return pixel_array;
};