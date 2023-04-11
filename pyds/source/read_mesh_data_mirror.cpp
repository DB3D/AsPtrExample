
// Example5: Access mesh data by creating mirror structs, 
//           These struct are more challenging to recreate than Example2, you'll need to be very carreful to not get anything wrong
//           Thanks for Luxcore plugin for doing the work
//           https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h
//           https://github.com/LuxCoreRender/LuxCore/blob/master/src/luxcore/pyluxcoreforblender.cpp

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <array>

//----------------------------------------------------------------------------------------------
// Mirror Struct
//----------------------------------------------------------------------------------------------

#ifndef _BLENDER_TYPES_H
#define _BLENDER_TYPES_H

namespace bl_types {

static const int ME_SMOOTH = 1;

//DNA_meshdata_types.h
typedef struct MLoopTri {
    unsigned int tri[3];
    unsigned int poly;
} MLoopTri;

//DNA_meshdata_types.h
typedef struct MLoopUV {
    float uv[2];
    int flag;
} MLoopUV;

//DNA_meshdata_types.h
typedef struct MLoopCol {
    unsigned char r, g, b, a;
} MLoopCol;

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

//RE_pipeline.h
struct RenderPass {
    struct RenderPass *next, *prev;
    int channels;
    char name[64];
    char chan_id[8];
    float *rect;  // The only thing we are interested in == the pixesl
    int rectx, recty;
    char fullname[64];
    char view[64];
    int view_id;
    int pad;
};

//mathutils_Matrix.h
typedef struct {
    PyObject_VAR_HEAD float *matrix;  // The only thing we are interested in
    PyObject *cb_user;
    unsigned char cb_type;
    unsigned char cb_subtype;
    unsigned char flag;
    unsigned short num_col;
    unsigned short num_row;
} MatrixObject;

#define DNA_DEPRECATED

// Forward declarations of irrelevant structs, we only use them by pointers
struct Library;
struct IDProperty;
struct IDOverrideLibrary;
struct AssetMetaData;
struct LibraryWeakReference;
struct ID_Runtime;
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

//BLI_span.hh (don't touch these, eek)
template<typename T> class Span;
template<typename T> class MutableSpan;

struct MeshRuntimeHandle;

//DNA_listBase.h
typedef struct ListBase {
    void* first, * last;
} ListBase;

//DNA_ID.h
typedef struct ID_Runtime_Remap {
  int status;
  int skipped_refcounted;
  int skipped_direct;
  int skipped_indirect;
} ID_Runtime_Remap;

//DNA_ID.h
typedef struct ID_Runtime {
  ID_Runtime_Remap remap;
} ID_Runtime;

//DNA_ID.h
typedef struct ID {
  void* next, * prev;
  struct ID* newid;
  struct Library* lib;
  struct AssetMetaData *asset_data;
  char name[66];
  short flag;
  int tag;
  int us;
  int icon_id;
  unsigned int recalc;
  unsigned int recalc_up_to_undo_push;
  unsigned int recalc_after_undo_push;
  unsigned int session_uuid;

  IDProperty *properties;
  IDOverrideLibrary *override_library;

  struct ID *orig_id;

  void *py_instance;

  struct LibraryWeakReference *library_weak_reference;
  struct ID_Runtime runtime;
} ID;

//DNA_customdata_types.h
typedef struct CustomDataLayer {
    int type;
    int offset;
    int flag;
    int active;
    int active_rnd;
    int active_clone;
    int active_mask;
    int uid;
    char name[64];
    void* data;
} CustomDataLayer;

//DNA_customdata_types.h
typedef struct CustomData {
    CustomDataLayer* layers;
    int typemap[52];
    char _pad[4];
    int totlayer, maxlayer;
    int totsize;
    struct BLI_mempool* pool;
    CustomDataExternal* external;
} CustomData;

//DNA_mesh_types.h
typedef struct Mesh {
    ID id;
    struct AnimData* adt;
    struct Ipo* ipo DNA_DEPRECATED;
    struct Key* key;
    struct Material** mat;
    int totvert;
    int totedge;
    int totpoly;
    int totloop;
    CustomData vdata, edata, pdata, ldata;
    ListBase vertex_group_names;

    int vertex_group_active_index;
    int attributes_active_index;

    struct BMEditMesh* edit_mesh;
    struct MSelect* mselect;

    int totselect;
    int act_face;

    struct Mesh* texcomesh;

    float loc[3];
    float size[3];
    char texflag;

    char editflag;

    uint16_t flag;

    float smoothresh;

    char symmetry;
    char remesh_mode;
    short totcol;

    char cd_flag DNA_DEPRECATED;
    char subdiv DNA_DEPRECATED;
    char subdivr DNA_DEPRECATED;
    char subsurftype DNA_DEPRECATED;

    struct MPoly* mpoly DNA_DEPRECATED;
    struct MLoop* mloop DNA_DEPRECATED;
    struct MVert* mvert DNA_DEPRECATED;
    struct MEdge* medge DNA_DEPRECATED;
    struct MDeformVert* dvert DNA_DEPRECATED;
    struct MTFace* mtface DNA_DEPRECATED;
    struct TFace* tface DNA_DEPRECATED;
    struct MCol* mcol DNA_DEPRECATED;
    struct MFace* mface DNA_DEPRECATED;

    CustomData fdata;

    int totface;

    float remesh_voxel_size;
    float remesh_voxel_adaptivity;
    int face_sets_color_seed;
    int face_sets_color_default;
    char _pad1[4];

    MeshRuntimeHandle* runtime;

    //after that, there are spans, we ignore them..
} Mesh;

//DNA_customdata_types.h
typedef enum CustomDataType {
    CD_AUTO_FROM_NAME = -1,
    CD_MVERT = 0,
    CD_MSTICKY = 1, /* DEPRECATED */
    CD_MDEFORMVERT = 2,
    CD_MEDGE = 3,
    CD_MFACE = 4,
    CD_MTFACE = 5,
    CD_MCOL = 6,
    CD_ORIGINDEX = 7,
    CD_NORMAL = 8,
    CD_FACEMAP = 9, /* exclusive face group, each face can only be part of one */
    CD_PROP_FLOAT = 10,
    CD_PROP_INT32 = 11,
    CD_PROP_STRING = 12,
    CD_ORIGSPACE = 13, /* for modifier stack face location mapping */
    CD_ORCO = 14,      /* undeformed vertex coordinates, normalized to 0..1 range */
    CD_MTEXPOLY = 15, /* deprecated */
    CD_MLOOPUV = 16,
    CD_PROP_BYTE_COLOR = 17,
    CD_TANGENT = 18,
    CD_MDISPS = 19,
    CD_PREVIEW_MCOL = 20, /* For displaying weight-paint colors. */
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

//------------------------------------------------------------------------------
// Blender struct access functions
//------------------------------------------------------------------------------

template<typename CustomData>
static int CustomData_get_active_layer_index(const CustomData *data, int type)
{
	const int layer_index = data->typemap[type];
	return (layer_index != -1) ? layer_index + data->layers[layer_index].active : -1;
}

template<typename CustomData>
static int CustomData_get_named_layer_index(const CustomData* data, const int type, const char* name)
{
	cout << "Tot layer: " << data->totlayer << endl;

	for (int i = 0; i < data->totlayer; i++) {
		cout << i << ":" << data->layers[i].name << ", " << data->layers[i].type << endl;
		if (data->layers[i].type == type) {
			if (STREQ(data->layers[i].name, name)) {
				return i;
			}
		}
	}

	return -1;
}

template<typename CustomData>
static void *CustomData_get_layer(const CustomData *data, int type)
{
	/* get the layer index of the active layer of type */
	int layer_index = CustomData_get_active_layer_index(data, type);
	if (layer_index == -1) {
		return nullptr;
	}

	return data->layers[layer_index].data;
}

template<typename CustomData>
static void* CustomData_get_layer_named(const CustomData* data, int type, const char *name)
{
	/* get the layer index of the active layer of type */
	int layer_index = CustomData_get_named_layer_index(data, type, name);
	if (layer_index == -1) {
		return nullptr;
	}

	return data->layers[layer_index].data;
}

//----------------------------------------------------------------------------------------------
// Main function:
//----------------------------------------------------------------------------------------------

// Main function:

static PyObject* read_mesh_data_mirror(PyObject* self, PyObject* args) {
    unsigned long long address_int;

    // check if args are correct
    if (!PyArg_ParseTuple(args, "K", &address_int)) {
        return nullptr;
    }

    std::cout << "running read_mesh_data_mirror in C++" << std::endl;

    // interpret memory
    uintptr_t address = static_cast<uintptr_t>(address_int);
    bl_types::bl_34::Mesh* mesh = reinterpret_cast<bl_types::bl_34::Mesh*>(address);

    // example on reading simple properties
    std::cout << " mesh->id.name : " << mesh->id.name << std::endl;
    std::cout << " mesh->totedge : " << mesh->totedge << std::endl;

    // example reading first 3 vertices co
    bl_types::MEdge *edata = static_cast<bl_types::MEdge *>(CustomData_get_layer(&mesh->edata, bl_types::bl_34::CD_MEDGE));
    if (edata != nullptr && mesh->totedge >= 3) {
        for (int i = 0; i < 3; ++i) {
            std::cout << " edata[" << i << "].v : (" <<  edata[i].v1 << ", " << edata[i].v2 << ")" << std::endl;
        };
    };
    //return None, this function just print in console
    Py_RETURN_NONE;
};
