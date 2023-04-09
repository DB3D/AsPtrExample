
// EXAMPLE 3 (WIP)

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>

#include <MEM_guardedalloc.h>
#include <BLI_span.hh>
#include <DNA_mesh_types.h>

//////////////////////////////////////////////////////
// Mirrorring "struct Mesh" Luxcore method
//////////////////////////////////////////////////////

/*
Note struct Mesh is quite hard to use outside of blender source code! it has quite a few dependencies
https://github.com/blender/blender/blob/main/source/blender/makesdna/DNA_mesh_types.h
#include "DNA_mesh_types.h"  // found in: ../3.4/source/blender/makesdna

Here i followed the luxcore method, consisting of mirrorring the same struct as blender
https://devtalk.blender.org/t/render-engine-add-ons-performance/19251/9
https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h
We are following this implementation in this file

PLEASE NOTE this method require much more maintenance and understanding of source code than `_read_mesh_elements.cpp`
*/

// // TODO, instead of luxcore method, let's re-try to include paths we need, we might need to build blender first and include a whole bunch of libraries
// // for now, will comment whole mirror-block below

// #ifndef _BLENDER_TYPES_H
// #define _BLENDER_TYPES_H

// namespace bl_types {

// static const int ME_SMOOTH = 1;

// typedef struct MLoopTri {
//     unsigned int tri[3];
//     unsigned int poly;
// } MLoopTri;

// typedef struct MLoopUV {
//     float uv[2];
//     int flag;
// } MLoopUV;

// typedef struct MLoopCol {
//     unsigned char r, g, b, a;
// } MLoopCol;

// /* Mesh Vertices. Typically accessed from #Mesh.verts()*/
// typedef struct MVert {
//   float co[3];
//   /*Deprecated flag for storing hide status and selection, which are now stored in separate generic attributes. Kept for file read and write. */
//   char flag_legacy;
//   /* Deprecated bevel weight storage, now located in #CD_BWEIGHT, except for file read and write. */
//   char bweight_legacy;
//   char _pad[2];
// } MVert;

// /* Mesh Edges. Typically accessed with #Mesh.edges() */
// typedef struct MEdge {
//   /* Un-ordered vertex indices (cannot match). */
//   unsigned int v1, v2;
//   /* Deprecated edge crease, now located in #CD_CREASE, except for file read and write. */
//   char crease_legacy;
//   /*Deprecated bevel weight storage, now located in #CD_BWEIGHT, except for file read and write.*/
//   char bweight_legacy;
//   short flag;
// } MEdge;

// /* Mesh Faces.This only stores the polygon size & flags, the vertex & edge indices are stored in the #MLoop. Typically accessed with #Mesh.polys(). */
// typedef struct MPoly {
//   /* Offset into loop array and number of loops in the face. */
//   int loopstart;
//   /* Keep signed since we need to subtract when getting the previous loop. */
//   int totloop;
//   /* Deprecated material index. Now stored in the "material_index" attribute, but kept for IO. */
//   short mat_nr_legacy;
//   char flag, _pad;
// } MPoly;

// /* Mesh Face Corners. "Loop" is an internal name for the corner of a polygon (#MPoly). Typically accessed with #Mesh.loops(). */
// typedef struct MLoop {
//   /* Vertex index into an #MVert array. */
//   unsigned int v;
//   /* Edge index into an #MEdge array. */
//   unsigned int e;
// } MLoop;

// struct RenderPass {
//     struct RenderPass *next, *prev;
//     int channels;
//     char name[64];
//     char chan_id[8];
//     float *rect;  // The only thing we are interested in
//     int rectx, recty;

//     char fullname[64];
//     char view[64];
//     int view_id;

//     int pad;
// };

// // from blender/source/blender/python/mathutils/mathutils_Matrix.h
// typedef struct {
//     // The following is expanded form of the macro BASE_MATH_MEMBERS(matrix)
//     PyObject_VAR_HEAD float *matrix;  // The only thing we are interested in
//     PyObject *cb_user;
//     unsigned char cb_type;
//     unsigned char cb_subtype;
//     unsigned char flag;

//     unsigned short num_col;
//     unsigned short num_row;
// } MatrixObject;

// // Mesh and dependencies (required for fast custom split normals support)

// // List of relevant files (as one line so it can be copy/pasted to open all files quickly in an editor)
// // blender/source/blender/makesdna/DNA_ID.h blender/source/blender/makesdna/DNA_customdata_types.h blender/source/blender/makesdna/DNA_mesh_types.h

// #define DNA_DEPRECATED

// // Forward declarations of irrelevant structs, we only use them by pointers
// struct Library;
// struct IDProperty;
// struct IDOverrideLibrary;
// struct AssetMetaData;
// struct LibraryWeakReference;
// struct ID_Runtime;
// struct BLI_mempool;
// struct CustomDataExternal;
// struct EditMeshData;
// struct SubdivCCG;
// struct LinkNode;
// struct ShrinkwrapBoundaryData;
// struct AnimData;
// struct Ipo;
// struct Key;
// struct Material;
// struct MSelect;
// struct MFace;
// struct MTFace;
// struct TFace;
// struct MEdge;
// struct MDeformVert;
// struct MCol;
// struct BMEditMesh;
// struct Multires;

// namespace bl_34 {   

// // template<typename T> class Span;
// // template<typename T> class MutableSpan;

// struct MeshRuntimeHandle;
// class AttributeAccessor;
// class MutableAttributeAccessor;

// // From blender/source/blender/makesdna/DNA_listBase.h
// typedef struct ListBase {
//     void* first, * last;
// } ListBase;

// // From blender/source/blender/makesdna/DNA_ID.h

// typedef struct ID_Runtime_Remap {
//   /** Status during ID remapping. */
//   int status;
//   /** During ID remapping the number of skipped use cases that refcount the data-block. */
//   int skipped_refcounted;
//   /**
//    * During ID remapping the number of direct use cases that could be remapped
//    * (e.g. obdata when in edit mode).
//    */
//   int skipped_direct;
//   /** During ID remapping, the number of indirect use cases that could not be remapped. */
//   int skipped_indirect;
// } ID_Runtime_Remap;

// typedef struct ID_Runtime {
//   ID_Runtime_Remap remap;
// } ID_Runtime;

// typedef struct ID {
//     void* next, * prev;
//     struct ID* newid;

//     struct Library* lib;

//     /** If the ID is an asset, this pointer is set. Owning pointer. */
//     struct AssetMetaData *asset_data;

//     /** MAX_ID_NAME. */
//     char name[66];
//     /**
//      * LIB_... flags report on status of the data-block this ID belongs to
//      * (persistent, saved to and read from .blend).
//      */
//     short flag;
//     /**
//      * LIB_TAG_... tags (runtime only, cleared at read time).
//      */
//     int tag;
//     int us;
//     int icon_id;
//     unsigned int recalc;
//     /**
//      * Used by undo code. recalc_after_undo_push contains the changes between the
//      * last undo push and the current state. This is accumulated as IDs are tagged
//      * for update in the depsgraph, and only cleared on undo push.
//      *
//      * recalc_up_to_undo_push is saved to undo memory, and is the value of
//      * recalc_after_undo_push at the time of the undo push. This means it can be
//      * used to find the changes between undo states.
//      */
//     unsigned int recalc_up_to_undo_push;
//     unsigned int recalc_after_undo_push;
    
//     /**
//      * A session-wide unique identifier for a given ID, that remain the same across potential
//      * re-allocations (e.g. due to undo/redo steps).
//      */
//     unsigned int session_uuid;

//   IDProperty *properties;

//   /** Reference linked ID which this one overrides. */
//   IDOverrideLibrary *override_library;

//   /**
//    * Only set for data-blocks which are coming from copy-on-write, points to
//    * the original version of it.
//    * Also used temporarily during memfile undo to keep a reference to old ID when found.
//    */
//   struct ID *orig_id;

//   /**
//    * Holds the #PyObject reference to the ID (initialized on demand).
//    *
//    * This isn't essential, it could be removed however it gives some advantages:
//    *
//    * - Every time the #ID is accessed a #BPy_StructRNA doesn't have to be created & destroyed
//    *   (consider all the polling and drawing functions that access ID's).
//    *
//    * - When this #ID is deleted, the #BPy_StructRNA can be invalidated
//    *   so accessing it from Python raises an exception instead of crashing.
//    *
//    *   This is of limited benefit though, as it doesn't apply to non #ID data
//    *   that references this ID (the bones of an armature or the modifiers of an object for e.g.).
//    */
//   void *py_instance;

//   /**
//    * Weak reference to an ID in a given library file, used to allow re-using already appended data
//    * in some cases, instead of appending it again.
//    *
//    * May be NULL.
//    */
//   struct LibraryWeakReference *library_weak_reference;

//   struct ID_Runtime runtime;
// } ID;

// typedef struct CustomDataLayer {
//     /** Type of data in layer. */
//     int type;
//     /** In editmode, offset of layer in block. */
//     int offset;
//     /** General purpose flag. */
//     int flag;
//     /** Number of the active layer of this type. */
//     int active;
//     /** Number of the layer to render. */
//     int active_rnd;
//     /** Number of the layer to render. */
//     int active_clone;
//     /** Number of the layer to render. */
//     int active_mask;
//     /** Shape keyblock unique id reference. */
//     int uid;
//     /** Layer name, MAX_CUSTOMDATA_LAYER_NAME. */
//     char name[64];
//     /** Layer data. */
//     void* data;
// } CustomDataLayer;

// typedef struct CustomData {
//     /** CustomDataLayers, ordered by type. */
//     CustomDataLayer* layers;
//     /**
//      * runtime only! - maps types to indices of first layer of that type,
//      * MUST be >= CD_NUMTYPES, but we can't use a define here.
//      * Correct size is ensured in CustomData_update_typemap assert().
//      */
//     int typemap[52];
//     char _pad[4];
//     /** Number of layers, size of layers array. */
//     int totlayer, maxlayer;
//     /** In editmode, total size of all data layers. */
//     int totsize;
//     /** (BMesh Only): Memory pool for allocation of blocks. */
//     struct BLI_mempool* pool;
//     /** External file storing customdata layers. */
//     CustomDataExternal* external;
// } CustomData;

// typedef struct Mesh {
//     ID id;
//     /** Animation data (must be immediately after id for utilities to use it). */
//     struct AnimData* adt;

//     /** Old animation system, deprecated for 2.5. */
//     struct Ipo* ipo DNA_DEPRECATED;
//     struct Key* key;

//     /**
//      * An array of materials, with length #totcol. These can be overridden by material slots
//      * on #Object. Indices in the "material_index" attribute control which material is used for every
//      * face.
//      */
//     struct Material** mat;

//     /** The number of vertices (#MVert) in the mesh, and the size of #vdata. */
//     int totvert;
//     /** The number of edges (#MEdge) in the mesh, and the size of #edata. */
//     int totedge;
//     /** The number of polygons/faces (#MPoly) in the mesh, and the size of #pdata. */
//     int totpoly;
//     /** The number of face corners (#MLoop) in the mesh, and the size of #ldata. */
//     int totloop;

//     CustomData vdata, edata, pdata, ldata;

//     /**
//      * List of vertex group (#bDeformGroup) names and flags only. Actual weights are stored in dvert.
//      * \note This pointer is for convenient access to the #CD_MDEFORMVERT layer in #vdata.
//      */
//     ListBase vertex_group_names;
//     /** The active index in the #vertex_group_names list. */
//     int vertex_group_active_index;

//     /**
//      * The index of the active attribute in the UI. The attribute list is a combination of the
//      * generic type attributes from vertex, edge, face, and corner custom data.
//      */
//     int attributes_active_index;

//     /**
//      * Runtime storage of the edit mode mesh. If it exists, it generally has the most up-to-date
//      * information about the mesh.
//      * \note When the object is available, the preferred access method is #BKE_editmesh_from_object.
//      */
//     struct BMEditMesh* edit_mesh;

//     /**
//      * This array represents the selection order when the user manually picks elements in edit-mode,
//      * some tools take advantage of this information. All elements in this array are expected to be
//      * selected, see #BKE_mesh_mselect_validate which ensures this. For procedurally created meshes,
//      * this is generally empty (selections are stored as boolean attributes in the corresponding
//      * custom data).
//      */
//     struct MSelect* mselect;

//     /** The length of the #mselect array. */
//     int totselect;

//     /**
//      * In most cases the last selected element (see #mselect) represents the active element.
//      * For faces we make an exception and store the active face separately so it can be active
//      * even when no faces are selected. This is done to prevent flickering in the material properties
//      * and UV Editor which base the content they display on the current material which is controlled
//      * by the active face.
//      *
//      * \note This is mainly stored for use in edit-mode.
//      */
//     int act_face;

//     /**
//      * An optional mesh owned elsewhere (by #Main) that can be used to override
//      * the texture space #loc and #size.
//      * \note Vertex indices should be aligned for this to work usefully.
//      */
//     struct Mesh* texcomesh;

//     /** Texture space location and size, used for procedural coordinates when rendering. */
//     float loc[3];
//     float size[3];
//     char texflag;

//     /** Various flags used when editing the mesh. */
//     char editflag;
//     /** Mostly more flags used when editing or displaying the mesh. */
//     uint16_t flag;

//     /**
//      * The angle for auto smooth in radians. `M_PI` (180 degrees) causes all edges to be smooth.
//      */
//     float smoothresh;

//     /**
//      * User-defined symmetry flag (#eMeshSymmetryType) that causes editing operations to maintain
//      * symmetrical geometry. Supported by operations such as transform and weight-painting.
//      */
//     char symmetry;

//     /** Choice between different remesh methods in the UI. */
//     char remesh_mode;

//     /** The length of the #mat array. */
//     short totcol;

//     /**
//      * Deprecated flag for choosing whether to store specific custom data that was built into #Mesh
//      * structs in edit mode. Replaced by separating that data to separate layers. Kept for forward
//      * and backwards compatibility.
//      */
//     char cd_flag DNA_DEPRECATED;
//     char subdiv DNA_DEPRECATED;
//     char subdivr DNA_DEPRECATED;
//     char subsurftype DNA_DEPRECATED;

//     /** Deprecated pointer to mesh polygons, kept for forward compatibility. */
//     struct MPoly* mpoly DNA_DEPRECATED;
//     /** Deprecated pointer to face corners, kept for forward compatibility. */
//     struct MLoop* mloop DNA_DEPRECATED;

//     /** Deprecated array of mesh vertices, kept for reading old files, now stored in #CustomData. */
//     struct MVert* mvert DNA_DEPRECATED;
//     /** Deprecated array of mesh edges, kept for reading old files, now stored in #CustomData. */
//     struct MEdge* medge DNA_DEPRECATED;
//     /** Deprecated "Vertex group" data. Kept for reading old files, now stored in #CustomData. */
//     struct MDeformVert* dvert DNA_DEPRECATED;
//     /** Deprecated runtime data for tessellation face UVs and texture, kept for reading old files. */
//     struct MTFace* mtface DNA_DEPRECATED;
//     /** Deprecated, use mtface. */
//     struct TFace* tface DNA_DEPRECATED;
//     /** Deprecated array of colors for the tessellated faces, kept for reading old files. */
//     struct MCol* mcol DNA_DEPRECATED;
//     /** Deprecated face storage (quads & triangles only). Kept for reading old files. */
//     struct MFace* mface DNA_DEPRECATED;

//     /**
//      * Deprecated storage of old faces (only triangles or quads).
//      *
//      * \note This would be marked deprecated, however the particles still use this at run-time
//      * for placing particles on the mesh (something which should be eventually upgraded).
//      */
//     CustomData fdata;
//     /* Deprecated size of #fdata. */
//     int totface;

//     /** Per-mesh settings for voxel remesh. */
//     float remesh_voxel_size;
//     float remesh_voxel_adaptivity;

//     int face_sets_color_seed;
//     /* Stores the initial Face Set to be rendered white. This way the overlay can be enabled by
//     * default and Face Sets can be used without affecting the color of the mesh. */
//     int face_sets_color_default;

//     char _pad1[4];

//     /**
//      * Data that isn't saved in files, including caches of derived data, temporary data to improve
//      * the editing experience, etc. The struct is created when reading files and can be accessed
//      * without null checks, with the exception of some temporary meshes which should allocate and
//      * free the data if they are passed to functions that expect run-time data.
//      */
//     MeshRuntimeHandle* runtime;

//     /**
//      * Array of vertex positions (and various other data). Edges and faces are defined by indices
//      * into this array.
//      */
//     blender::Span<MVert> verts() const;
//     /** Write access to vertex data. */
//     blender::MutableSpan<MVert> verts_for_write();
//     /**
//      * Array of edges, containing vertex indices. For simple triangle or quad meshes, edges could be
//      * calculated from the #MPoly and #MLoop arrays, however, edges need to be stored explicitly to
//      * edge domain attributes and to support loose edges that aren't connected to faces.
//      */
//     blender::Span<MEdge> edges() const;
//     /** Write access to edge data. */
//     blender::MutableSpan<MEdge> edges_for_write();
//     /**
//      * Face topology storage of the size and offset of each face's section of the face corners.
//      */
//     blender::Span<MPoly> polys() const;
//     /** Write access to polygon data. */
//     blender::MutableSpan<MPoly> polys_for_write();
//     /**
//      * Mesh face corners that "loop" around each face, storing the vertex index and the index of the
//      * subsequent edge.
//      */
//     blender::Span<MLoop> loops() const;
//     /** Write access to loop data. */
//     blender::MutableSpan<MLoop> loops_for_write();

//     AttributeAccessor attributes() const;
//     MutableAttributeAccessor attributes_for_write();

//     /**
//      * Vertex group data, encoded as an array of indices and weights for every vertex.
//      * \warning: May be empty.
//      */
//     blender::Span<MDeformVert> deform_verts() const;
//     /** Write access to vertex group data. */
//     blender::MutableSpan<MDeformVert> deform_verts_for_write();

//     /**
//      * Cached triangulation of the mesh.
//      */
//     blender::Span<MLoopTri> looptris() const;

// } Mesh;

// /** #CustomData.type */
// typedef enum CustomDataType {
//     /* Used by GLSL attributes in the cases when we need a delayed CD type
//      * assignment (in the cases when we don't know in advance which layer
//      * we are addressing).
//      */
//     CD_AUTO_FROM_NAME = -1,

//     CD_MVERT = 0,
// #ifdef DNA_DEPRECATED_ALLOW
//     CD_MSTICKY = 1, /* DEPRECATED */
// #endif
//     CD_MDEFORMVERT = 2,
//     CD_MEDGE = 3,
//     CD_MFACE = 4,
//     CD_MTFACE = 5,
//     CD_MCOL = 6,
//     CD_ORIGINDEX = 7,
//     /**
//      * Used for derived face corner normals on mesh `ldata`, since currently they are not computed
//      * lazily. Derived vertex and polygon normals are stored in #Mesh_Runtime.
//      */
//      CD_NORMAL = 8,
//      CD_FACEMAP = 9, /* exclusive face group, each face can only be part of one */
//      CD_PROP_FLOAT = 10,
//      CD_PROP_INT32 = 11,
//      CD_PROP_STRING = 12,
//      CD_ORIGSPACE = 13, /* for modifier stack face location mapping */
//      CD_ORCO = 14,      /* undeformed vertex coordinates, normalized to 0..1 range */
// #ifdef DNA_DEPRECATED_ALLOW
//      CD_MTEXPOLY = 15, /* deprecated */
// #endif
//      CD_MLOOPUV = 16,
//      CD_PROP_BYTE_COLOR = 17,
//      CD_TANGENT = 18,
//      CD_MDISPS = 19,
//      CD_PREVIEW_MCOL = 20,           /* For displaying weight-paint colors. */
//                                      /*  CD_ID_MCOL          = 21, */
//      /* CD_TEXTURE_MLOOPCOL = 22, */ /* UNUSED */
//      CD_CLOTH_ORCO = 23,
//      /* CD_RECAST = 24, */ /* UNUSED */

//      CD_MPOLY = 25,
//      CD_MLOOP = 26,
//      CD_SHAPE_KEYINDEX = 27,
//      CD_SHAPEKEY = 28,
//      CD_BWEIGHT = 29,
//      /** Subdivision sharpness data per edge or per vertex. */
//      CD_CREASE = 30,
//      CD_ORIGSPACE_MLOOP = 31,
//      CD_PREVIEW_MLOOPCOL = 32,
//      CD_BM_ELEM_PYPTR = 33,

//      CD_PAINT_MASK = 34,
//      CD_GRID_PAINT_MASK = 35,
//      CD_MVERT_SKIN = 36,
//      CD_FREESTYLE_EDGE = 37,
//      CD_FREESTYLE_FACE = 38,
//      CD_MLOOPTANGENT = 39,
//      CD_TESSLOOPNORMAL = 40,
//      CD_CUSTOMLOOPNORMAL = 41,
//      CD_SCULPT_FACE_SETS = 42,

//      /* CD_LOCATION = 43, */ /* UNUSED */
//      /* CD_RADIUS = 44, */   /* UNUSED */
//      CD_PROP_INT8 = 45,
//      /* CD_HAIRMAPPING = 46, */ /* UNUSED, can be reused. */

//      CD_PROP_COLOR = 47,
//      CD_PROP_FLOAT3 = 48,
//      CD_PROP_FLOAT2 = 49,
//      CD_PROP_BOOL = 50,

//      CD_HAIRLENGTH = 51,

//      CD_NUMTYPES = 52,
// } CustomDataType;

// } // end namespace bl_34
// } // end namespace bl_types

// #endif


//////////////////////////////////////////////////////
// Define pyd function 
//////////////////////////////////////////////////////


static PyObject* read_mesh_data(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }

    std::cout << "running read_mesh_data in C++" << std::endl;

    uintptr_t address = static_cast<uintptr_t>(address_int);
    Mesh* mesh = reinterpret_cast<Mesh*>(address);

    //test prints
    std::cout << "read_mesh_data() -> mesh totvert:" << mesh->totvert << std::endl;
    std::cout << "read_mesh_data() -> mesh name:" << mesh->id.name << std::endl;

    // Access Mesh data basic attr
    auto vertices = mesh->verts(); //will return a Span, which is deeply integrated within blender internals.. Digging deep to get there..
    int num_verts = vertices.size();

    // Access mesh data mesh attr
    for (int i = 0; i < num_verts; ++i) {
        
        // Access vertex coordinates
        float x = vertices[i].co[0];
        float y = vertices[i].co[1];
        float z = vertices[i].co[2];

        // Print coordinates
        std::cout << "read_mesh_data() v.co : (" << x << ", " << y << ", " << z << ")" << std::endl;
    }

    //return totverts
    int r = mesh->totvert; 
    return Py_BuildValue("i", r);
}