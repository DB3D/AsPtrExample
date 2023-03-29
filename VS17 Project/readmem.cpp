
#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

//////////////////////////////////////////////////////////
//PROCEED TO MATCH OR INCLUDE BLENDER STRUCT 
//////////////////////////////////////////////////////////

/*
We are relying on .pyd to read blender memory, see "_FailedAttempt" folder
If you never built your own .pyd before, here's a simpler project that adds two int together:
https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp

In order to read in blender memory, we need to be able tu use blender internal struct that can be found in the C API!
This step can be challenging, you will need to search in blender code base, and find where these structs are defined, 
you will also need to make sure that the struct you found correspond to the python struct you are calling as_pointer() from

Once done, you could, try to match these structs from scratch
For example, this developer recreated a spline struct using Ctypes https://devtalk.blender.org/t/getting-structure-from-bpy-types-spline-as-pointer-with-ctypes/11759
but that might be challenging depending on the struct and their dependencies with other modules. 

So what i advise is to set up your IDE so your project can search for the files it needs in blender source code (via external dependencies)
How To do so in VisualStudio2017: 
 -In "Solution Explorer" right click on yourProject, go in properties
 -"ConfigurationProperties>C++>General>AdditionalIncludeDirectories" add the path to the folder with the files you need!(separate the paths with ;)
  in the context of this project i added the following paths:
    - ../3.4/source/
    - ../3.4/source/blender/
    - ../3.4/source/blender/makesdna
    - ../3.4/source/blender/makesdna/intern
    - ../3.4/source/blender/blenlib
    - ../3.4/source/blender/blenkernel
    - ../3.4/source/blender/blenfont
    - ../3.4/source/blender/gpu

Depending on the project, you might need to add more external dependencies, not only leading the the .h you want to include, but also on their own dependencies
Additionally, make sure your C++ langange is set to "ISO C++17 Standard (/std:c++17)", needed for blender source code
*/

//Including "struct Object"

#include "DNA_object_types.h"//found in: ../3.4/source/blender/makesdna
#include "BKE_main.h"        //found in: ../3.4/source/blender/blenkernel
#include "BKE_object.h"      //found in: ../3.4/source/blender/blenkernel
#include "BLI_utildefines.h" //found in: ../3.4/source/blender/blenlib


// Mirrorring "struct Mesh" 

/*
Note struct Mesh is very difficult to include! has quite a few dependencies, didn't manage to make it work this time
https://github.com/blender/blender/blob/main/source/blender/makesdna/DNA_mesh_types.h
#include "DNA_mesh_types.h"  // found in: ../3.4/source/blender/makesdna

we need to match it with our own struct instead, LuxCore managed to do just that
https://devtalk.blender.org/t/render-engine-add-ons-performance/19251/9
https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h
and what will follow is a similar implementation 
*/

#ifndef _BLENDER_TYPES_H
#define _BLENDER_TYPES_H

namespace bl_types {

//------------------------------------------------------------------------------
// Blender definitions and structures
//------------------------------------------------------------------------------

static const int ME_SMOOTH = 1;

typedef struct MLoopTri {
    unsigned int tri[3];
    unsigned int poly;
} MLoopTri;

typedef struct MLoopUV {
    float uv[2];
    int flag;
} MLoopUV;

typedef struct MLoopCol {
    unsigned char r, g, b, a;
} MLoopCol;

typedef struct MLoop {
    /** Vertex index. */
    unsigned int v;
    /** Edge index. */
    unsigned int e;
} MLoop;

typedef struct MPoly {
    /* offset into loop array and number of loops in the face */
    int loopstart;
    int totloop;
    short mat_nr;
    char flag, _pad;
} MPoly;

struct MVert {
    float co[3];
    char flag, bweight;
    char _pad[2];
};

struct RenderPass {
    struct RenderPass *next, *prev;
    int channels;
    char name[64];
    char chan_id[8];
    float *rect;  // The only thing we are interested in
    int rectx, recty;

    char fullname[64];
    char view[64];
    int view_id;

    int pad;
};

// from blender/source/blender/python/mathutils/mathutils_Matrix.h
typedef struct {
    // The following is expanded form of the macro BASE_MATH_MEMBERS(matrix)
    PyObject_VAR_HEAD float *matrix;  // The only thing we are interested in
    PyObject *cb_user;
    unsigned char cb_type;
    unsigned char cb_subtype;
    unsigned char flag;

    unsigned short num_col;
    unsigned short num_row;
} MatrixObject;


// ----------------------------------------------------------------------
// Mesh and dependencies (required for fast custom split normals support)

// List of relevant files (as one line so it can be copy/pasted to open all files quickly in an editor)
// blender/source/blender/makesdna/DNA_ID.h blender/source/blender/makesdna/DNA_customdata_types.h blender/source/blender/makesdna/DNA_mesh_types.h
// ----------------------------------------------------------------------

#define DNA_DEPRECATED

// Forward declarations of irrelevant structs
struct Library;
struct IDProperty;
struct IDOverrideLibrary;
struct BLI_mempool;
struct CustomDataExternal;
struct EditMeshData;
struct SubdivCCG;
struct LinkNode;
struct ShrinkwrapBoundaryData;
struct AnimData;
struct Ipo;
struct Key;
struct Material;
struct MSelect;
struct MFace;
struct MTFace;
struct TFace;
struct MEdge;
struct MDeformVert;
struct MCol;
struct BMEditMesh;
struct Multires;

namespace bl_34 {   

template<typename T> class Span;
template<typename T> class MutableSpan;

struct MeshRuntimeHandle;

// -------------------------------------------------------------------------------------
// From blender/source/blender/makesdna/DNA_listBase.h
typedef struct ListBase {
    void* first, * last;
} ListBase;
// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------
// From blender/source/blender/...
typedef struct ID {
    void* next, * prev;
    struct ID* newid;
    struct Library* lib;
    /** MAX_ID_NAME. */
    char name[66];
    /**
     * LIB_... flags report on status of the data-block this ID belongs to
     * (persistent, saved to and read from .blend).
     */
    short flag;
    /**
     * LIB_TAG_... tags (runtime only, cleared at read time).
     */
    int tag;
    int us;
    int icon_id;
    int recalc;
    char _pad[4];
    IDProperty* properties;

    /** Reference linked ID which this one overrides. */
    IDOverrideLibrary* override_library;

    /**
     * Only set for data-blocks which are coming from copy-on-write, points to
     * the original version of it.
     */
    struct ID* orig_id;

    void* py_instance;
} ID;
// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------
typedef struct CustomDataLayer {
    /** Type of data in layer. */
    int type;
    /** In editmode, offset of layer in block. */
    int offset;
    /** General purpose flag. */
    int flag;
    /** Number of the active layer of this type. */
    int active;
    /** Number of the layer to render. */
    int active_rnd;
    /** Number of the layer to render. */
    int active_clone;
    /** Number of the layer to render. */
    int active_mask;
    /** Shape keyblock unique id reference. */
    int uid;
    /** Layer name, MAX_CUSTOMDATA_LAYER_NAME. */
    char name[64];
    /** Layer data. */
    void* data;
} CustomDataLayer;

typedef struct CustomData {
    /** CustomDataLayers, ordered by type. */
    CustomDataLayer* layers;
    /**
     * runtime only! - maps types to indices of first layer of that type,
     * MUST be >= CD_NUMTYPES, but we can't use a define here.
     * Correct size is ensured in CustomData_update_typemap assert().
     */
    int typemap[52];
    char _pad[4];
    /** Number of layers, size of layers array. */
    int totlayer, maxlayer;
    /** In editmode, total size of all data layers. */
    int totsize;
    /** (BMesh Only): Memory pool for allocation of blocks. */
    struct BLI_mempool* pool;
    /** External file storing customdata layers. */
    CustomDataExternal* external;
} CustomData;

typedef struct Mesh {
    ID id;
    /** Animation data (must be immediately after id for utilities to use it). */
    struct AnimData* adt;

    /** Old animation system, deprecated for 2.5. */
    struct Ipo* ipo DNA_DEPRECATED;
    struct Key* key;

    /**
     * An array of materials, with length #totcol. These can be overridden by material slots
     * on #Object. Indices in the "material_index" attribute control which material is used for every
     * face.
     */
    struct Material** mat;

    /** The number of vertices (#MVert) in the mesh, and the size of #vdata. */
    int totvert;
    /** The number of edges (#MEdge) in the mesh, and the size of #edata. */
    int totedge;
    /** The number of polygons/faces (#MPoly) in the mesh, and the size of #pdata. */
    int totpoly;
    /** The number of face corners (#MLoop) in the mesh, and the size of #ldata. */
    int totloop;

    CustomData vdata, edata, pdata, ldata;

    /**
     * List of vertex group (#bDeformGroup) names and flags only. Actual weights are stored in dvert.
     * \note This pointer is for convenient access to the #CD_MDEFORMVERT layer in #vdata.
     */
    ListBase vertex_group_names;
    /** The active index in the #vertex_group_names list. */
    int vertex_group_active_index;

    /**
     * The index of the active attribute in the UI. The attribute list is a combination of the
     * generic type attributes from vertex, edge, face, and corner custom data.
     */
    int attributes_active_index;

    /**
     * Runtime storage of the edit mode mesh. If it exists, it generally has the most up-to-date
     * information about the mesh.
     * \note When the object is available, the preferred access method is #BKE_editmesh_from_object.
     */
    struct BMEditMesh* edit_mesh;

    /**
     * This array represents the selection order when the user manually picks elements in edit-mode,
     * some tools take advantage of this information. All elements in this array are expected to be
     * selected, see #BKE_mesh_mselect_validate which ensures this. For procedurally created meshes,
     * this is generally empty (selections are stored as boolean attributes in the corresponding
     * custom data).
     */
    struct MSelect* mselect;

    /** The length of the #mselect array. */
    int totselect;

    /**
     * In most cases the last selected element (see #mselect) represents the active element.
     * For faces we make an exception and store the active face separately so it can be active
     * even when no faces are selected. This is done to prevent flickering in the material properties
     * and UV Editor which base the content they display on the current material which is controlled
     * by the active face.
     *
     * \note This is mainly stored for use in edit-mode.
     */
    int act_face;

    /**
     * An optional mesh owned elsewhere (by #Main) that can be used to override
     * the texture space #loc and #size.
     * \note Vertex indices should be aligned for this to work usefully.
     */
    struct Mesh* texcomesh;

    /** Texture space location and size, used for procedural coordinates when rendering. */
    float loc[3];
    float size[3];
    char texflag;

    /** Various flags used when editing the mesh. */
    char editflag;
    /** Mostly more flags used when editing or displaying the mesh. */
    uint16_t flag;

    /**
     * The angle for auto smooth in radians. `M_PI` (180 degrees) causes all edges to be smooth.
     */
    float smoothresh;

    /** Per-mesh settings for voxel remesh. */
    float remesh_voxel_size;
    float remesh_voxel_adaptivity;

    int face_sets_color_seed;
    /* Stores the initial Face Set to be rendered white. This way the overlay can be enabled by
     * default and Face Sets can be used without affecting the color of the mesh. */
    int face_sets_color_default;

    /** The color attribute currently selected in the list and edited by a user. */
    char* active_color_attribute;
    /** The color attribute used by default (i.e. for rendering) if no name is given explicitly. */
    char* default_color_attribute;

    /**
     * User-defined symmetry flag (#eMeshSymmetryType) that causes editing operations to maintain
     * symmetrical geometry. Supported by operations such as transform and weight-painting.
     */
    char symmetry;

    /** Choice between different remesh methods in the UI. */
    char remesh_mode;

    /** The length of the #mat array. */
    short totcol;

    /**
     * Deprecated flag for choosing whether to store specific custom data that was built into #Mesh
     * structs in edit mode. Replaced by separating that data to separate layers. Kept for forward
     * and backwards compatibility.
     */
    char cd_flag DNA_DEPRECATED;
    char subdiv DNA_DEPRECATED;
    char subdivr DNA_DEPRECATED;
    char subsurftype DNA_DEPRECATED;

    /** Deprecated pointer to mesh polygons, kept for forward compatibility. */
    struct MPoly* mpoly DNA_DEPRECATED;
    /** Deprecated pointer to face corners, kept for forward compatibility. */
    struct MLoop* mloop DNA_DEPRECATED;

    /** Deprecated array of mesh vertices, kept for reading old files, now stored in #CustomData. */
    struct MVert* mvert DNA_DEPRECATED;
    /** Deprecated array of mesh edges, kept for reading old files, now stored in #CustomData. */
    struct MEdge* medge DNA_DEPRECATED;
    /** Deprecated "Vertex group" data. Kept for reading old files, now stored in #CustomData. */
    struct MDeformVert* dvert DNA_DEPRECATED;
    /** Deprecated runtime data for tessellation face UVs and texture, kept for reading old files. */
    struct MTFace* mtface DNA_DEPRECATED;
    /** Deprecated, use mtface. */
    struct TFace* tface DNA_DEPRECATED;
    /** Deprecated array of colors for the tessellated faces, kept for reading old files. */
    struct MCol* mcol DNA_DEPRECATED;
    /** Deprecated face storage (quads & triangles only). Kept for reading old files. */
    struct MFace* mface DNA_DEPRECATED;

    /**
     * Deprecated storage of old faces (only triangles or quads).
     *
     * \note This would be marked deprecated, however the particles still use this at run-time
     * for placing particles on the mesh (something which should be eventually upgraded).
     */
    CustomData fdata;
    /* Deprecated size of #fdata. */
    int totface;

    char _pad1[4];

    /**
     * Data that isn't saved in files, including caches of derived data, temporary data to improve
     * the editing experience, etc. The struct is created when reading files and can be accessed
     * without null checks, with the exception of some temporary meshes which should allocate and
     * free the data if they are passed to functions that expect run-time data.
     */
    MeshRuntimeHandle* runtime;
} Mesh;

/** #CustomData.type */
typedef enum CustomDataType {
    /* Used by GLSL attributes in the cases when we need a delayed CD type
     * assignment (in the cases when we don't know in advance which layer
     * we are addressing).
     */
    CD_AUTO_FROM_NAME = -1,

    CD_MVERT = 0,
#ifdef DNA_DEPRECATED_ALLOW
    CD_MSTICKY = 1, /* DEPRECATED */
#endif
    CD_MDEFORMVERT = 2,
    CD_MEDGE = 3,
    CD_MFACE = 4,
    CD_MTFACE = 5,
    CD_MCOL = 6,
    CD_ORIGINDEX = 7,
    /**
     * Used for derived face corner normals on mesh `ldata`, since currently they are not computed
     * lazily. Derived vertex and polygon normals are stored in #Mesh_Runtime.
     */
     CD_NORMAL = 8,
     CD_FACEMAP = 9, /* exclusive face group, each face can only be part of one */
     CD_PROP_FLOAT = 10,
     CD_PROP_INT32 = 11,
     CD_PROP_STRING = 12,
     CD_ORIGSPACE = 13, /* for modifier stack face location mapping */
     CD_ORCO = 14,      /* undeformed vertex coordinates, normalized to 0..1 range */
#ifdef DNA_DEPRECATED_ALLOW
     CD_MTEXPOLY = 15, /* deprecated */
#endif
     CD_MLOOPUV = 16,
     CD_PROP_BYTE_COLOR = 17,
     CD_TANGENT = 18,
     CD_MDISPS = 19,
     CD_PREVIEW_MCOL = 20,           /* For displaying weight-paint colors. */
                                     /*  CD_ID_MCOL          = 21, */
     /* CD_TEXTURE_MLOOPCOL = 22, */ /* UNUSED */
     CD_CLOTH_ORCO = 23,
     /* CD_RECAST = 24, */ /* UNUSED */

     CD_MPOLY = 25,
     CD_MLOOP = 26,
     CD_SHAPE_KEYINDEX = 27,
     CD_SHAPEKEY = 28,
     CD_BWEIGHT = 29,
     /** Subdivision sharpness data per edge or per vertex. */
     CD_CREASE = 30,
     CD_ORIGSPACE_MLOOP = 31,
     CD_PREVIEW_MLOOPCOL = 32,
     CD_BM_ELEM_PYPTR = 33,

     CD_PAINT_MASK = 34,
     CD_GRID_PAINT_MASK = 35,
     CD_MVERT_SKIN = 36,
     CD_FREESTYLE_EDGE = 37,
     CD_FREESTYLE_FACE = 38,
     CD_MLOOPTANGENT = 39,
     CD_TESSLOOPNORMAL = 40,
     CD_CUSTOMLOOPNORMAL = 41,
     CD_SCULPT_FACE_SETS = 42,

     /* CD_LOCATION = 43, */ /* UNUSED */
     /* CD_RADIUS = 44, */   /* UNUSED */
     CD_PROP_INT8 = 45,
     /* CD_HAIRMAPPING = 46, */ /* UNUSED, can be reused. */

     CD_PROP_COLOR = 47,
     CD_PROP_FLOAT3 = 48,
     CD_PROP_FLOAT2 = 49,
     CD_PROP_BOOL = 50,

     CD_HAIRLENGTH = 51,

     CD_NUMTYPES = 52,
} CustomDataType;

} // end namespace bl_34
} // end namespace bl_types

#endif


//////////////////////////////////////////////////////////
// PROCEED TO WRITE THE .PYD FUNCTIONS
//////////////////////////////////////////////////////////

//Get object name, pass C.object.as_pointer()
static PyObject* get_object_name(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }
    
    std::cout << "running get_object_name in C++" << std::endl;

    uintptr_t address = static_cast<uintptr_t>(address_int);
    Object* obj = reinterpret_cast<Object*>(address);

    std::string obj_name(obj->id.name + 2);

    return Py_BuildValue("s", obj_name.c_str());
}

//Get mesh vertcount, pass C.object.data.as_pointer()
static PyObject* count_mesh_verts(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }

    std::cout << "running count_mesh_verts in C++" << std::endl;

    uintptr_t address = static_cast<uintptr_t>(address_int);
    bl_types::bl_34::Mesh* mesh = reinterpret_cast<bl_types::bl_34::Mesh*>(address);

    std::cout << "mesh name:" << mesh->id.name << std::endl;

    int num_verts = mesh->totvert;
    return Py_BuildValue("i", num_verts);
}

//Define functions
static PyMethodDef SimpleAddMethods[] = {
    {"get_object_name", get_object_name, METH_VARARGS, "Get object name by its pointer."},
    {"count_mesh_verts", count_mesh_verts, METH_VARARGS, "Count the number of vertices in a Mesh struct."},
    {nullptr, nullptr, 0, nullptr}
};

//Define Modules
static struct PyModuleDef readmemmodule = {
    PyModuleDef_HEAD_INIT,
    "readmem",
    nullptr,
    -1,
    SimpleAddMethods
};

//Create the Module
extern "C" __declspec(dllexport) PyMODINIT_FUNC PyInit_readmem(void) {
    return PyModule_Create(&readmemmodule);
}

//Once .dll has been compiled in x64 folder, make a copy in "pydll" folder and rename it to "readmem.pyd"