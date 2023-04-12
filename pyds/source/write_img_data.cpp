/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example6: directly write image data pixels from memory 
//           Note that writing in memory in outside of valid range will crash blender, extreme caution is needed
//           Note that for this example, trying to recreate may be challengine, couldn't done it

#pragma once

#include <Python.h>
#include <iostream>
#include <cstdint>
#include <cstddef>

#include "DNA_image_types.h" // access struct Image
#include "IMB_imbuf.h" // for IMB_rect_from_float
#include "IMB_imbuf_types.h" // access struct ImBuf
#include "BKE_image.h" // access BKE_image_acquire_ibuf which is needed to get toward the pixels array in C API

// Main function:

static PyObject* write_img_data(PyObject* self, PyObject* args) {
    unsigned long long img_ptr;
    float alpha_strength;

    // Check if args are correct
    if (!PyArg_ParseTuple(args, "Kf", &img_ptr, &alpha_strength)) {
        return nullptr;
    }

    std::cout << "running write_img_data in C++" << std::endl;

    // Cast the pointers to the appropriate types
    Image* img = reinterpret_cast<Image*>(img_ptr);

    // Create an ImageUser structure, initialize it and set the appropriate values
    ImBuf *ibuf;
    void *lock; //dummy void and iuser
    ibuf = BKE_image_acquire_ibuf(img, nullptr, &lock);

    // asserts
    if (ibuf == nullptr) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to acquire ImBuf");
        return nullptr;
    }
    
    // test print some properties
    std::cout << " imBuf Width: " << ibuf->x << std::endl;
    std::cout << " imBuf Height: " << ibuf->y << std::endl;
    std::cout << " imBuf Channels: " << ibuf->channels << std::endl;

    // code derrived from image_ops.c image_invert_exec()

    size_t i;
    if (ibuf->rect_float) {
        std::cout << " imBuf redefine alpha ibuf->rect_float : " << ibuf->x << std::endl;

        float *fp = (float *)ibuf->rect_float;
        for (i = ((size_t)ibuf->x) * ibuf->y; i > 0; i--, fp += 4) {
            fp[3] = alpha_strength;
        }

        // why is this bit needed ??
        if (ibuf->rect) {
            IMB_rect_from_float(ibuf);
        }
    }
    else if (ibuf->rect) {
        std::cout << " imBuf redefine alpha ibuf->rect : " << ibuf->x << std::endl;

        char *cp = (char *)ibuf->rect;
        for (i = ((size_t)ibuf->x) * ibuf->y; i > 0; i--, cp += 4) {
            cp[3] = static_cast<char>(alpha_strength * 255);
        }
    }
    else {
        PyErr_SetString(PyExc_RuntimeError, "Failed to identify Rect type");
        return nullptr;
    }

    // tell blender this image data has changed ??
    ibuf->userflags |= IB_DISPLAY_BUFFER_INVALID;
    BKE_image_mark_dirty(img, ibuf);

    // send update signal ??
    BKE_image_partial_update_mark_full_update(img);

    //release image buffer
    BKE_image_release_ibuf(img, ibuf, lock);

    //return None
    Py_RETURN_NONE;
}


// I tried to use the mirror method earlier, but the rabbit hole goes too deep, once you need to replicate something in BKE it seems to be a dead end
/*

// replicate the img data elements structs 

// Forward declarations of irrelevant structs (we only use their pointers)
struct MovieCache;
struct GPUTexture;
struct PackedFile;
struct PreviewImage;
struct RenderResult;
struct Stereo3dFormat;
struct IDProperty;
struct IDOverrideLibrary;
struct PartialUpdateRegister;
struct PartialUpdateUser;
struct MEM_CacheLimiterHandle_s;
struct ColorSpace; 
struct ColormanageCache;
struct LibraryWeakReference;

#define DNA_DEPRECATED
#define IMB_MIPMAP_LEVELS 20
#define IMB_FILENAME_SIZE 1024

//DNA_vec_types.h
typedef struct rcti {
  int xmin, xmax;
  int ymin, ymax;
} rcti;

//IMB_imbuf_types.h
enum eImbFileType {
  IMB_FTYPE_PNG = 1,
  IMB_FTYPE_TGA = 2,
  IMB_FTYPE_JPG = 3,
  IMB_FTYPE_BMP = 4,
  IMB_FTYPE_OPENEXR = 5,
  IMB_FTYPE_IMAGIC = 6,
#ifdef WITH_OPENIMAGEIO
  IMB_FTYPE_PSD = 7,
#endif
#ifdef WITH_OPENJPEG
  IMB_FTYPE_JP2 = 8,
#endif
#ifdef WITH_HDR
  IMB_FTYPE_RADHDR = 9,
#endif
#ifdef WITH_TIFF
  IMB_FTYPE_TIF = 10,
#endif
#ifdef WITH_CINEON
  IMB_FTYPE_CINEON = 11,
  IMB_FTYPE_DPX = 12,
#endif
#ifdef WITH_DDS
  IMB_FTYPE_DDS = 13,
#endif
#ifdef WITH_WEBP
  IMB_FTYPE_WEBP = 14,
#endif
};

//IMB_imbuf_types.h
typedef struct DDSData {
  unsigned int fourcc;
  unsigned int nummipmaps;
  unsigned char *data;
  unsigned int size;
} DDSData;

//IMB_imbuf_types.h
typedef struct ImbFormatOptions {
  short flag;
  char quality;
} ImbFormatOptions;

//IMB_imbuf_types.h
typedef struct ImBuf {
  int x, y;
  unsigned char planes;
  int channels;

  int flags;
  int mall;

  unsigned int *rect;
  float *rect_float;

  double ppm[2];

  int tilex, tiley;
  int xtiles, ytiles;
  unsigned int **tiles;

  int *zbuf;
  float *zbuf_float;

  float dither;

  struct ImBuf *mipmap[IMB_MIPMAP_LEVELS];
  int miptot, miplevel;

  int index;
  int userflags;
  struct IDProperty *metadata;
  void *userdata;

  enum eImbFileType ftype;
  ImbFormatOptions foptions;
  char name[IMB_FILENAME_SIZE];
  char cachename[IMB_FILENAME_SIZE];

  struct MEM_CacheLimiterHandle_s *c_handle;
  int refcounter;

  unsigned char *encodedbuffer;
  unsigned int encodedsize;
  unsigned int encodedbuffersize;

  struct ColorSpace *rect_colorspace;
  struct ColorSpace *float_colorspace;
  unsigned int *display_buffer_flags;
  struct ColormanageCache *colormanage_cache;
  int colormanage_flag;
  rcti invalid_rect;

  struct DDSData dds_data;
} ImBuf;

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

//DNA_image_types.h
typedef struct Image_Runtime {
  void *cache_mutex; // Mutex used to guarantee thread-safe access to the cached ImBuf of the corresponding image ID.
                     // And this is the whole problem, in blender source code they use a function to access imBuf
  struct PartialUpdateRegister *partial_update_register;
  struct PartialUpdateUser *partial_update_user;
} Image_Runtime;

//DNA_color_types.h
typedef struct ColorManagedColorspaceSettings {
  char name[64];
} ColorManagedColorspaceSettings;

//DNA_image_types.h
typedef struct Image {
  ID id;
  char filepath[1024];

  struct MovieCache *cache;
  struct GPUTexture *gputexture[3][2];

  ListBase anims;
  struct RenderResult *rr;

  ListBase renderslots;
  short render_slot, last_render_slot;

  int flag;
  short source, type;
  int lastframe;

  int gpuframenr;
  short gpuflag;
  short gpu_pass;
  short gpu_layer;
  short gpu_view;
  char _pad2[4];

  struct PackedFile *packedfile DNA_DEPRECATED;
  struct ListBase packedfiles;
  struct PreviewImage *preview;

  int lastused;

  int gen_x DNA_DEPRECATED, gen_y DNA_DEPRECATED;
  char gen_type DNA_DEPRECATED, gen_flag DNA_DEPRECATED;
  short gen_depth DNA_DEPRECATED;
  float gen_color[4] DNA_DEPRECATED;

  float aspx, aspy;

  ColorManagedColorspaceSettings colorspace_settings;
  char alpha_mode;

  char _pad;

  char eye;
  char views_format;

  int active_tile_index;
  ListBase tiles;

  ListBase views;
  struct Stereo3dFormat *stereo3d_format;

  Image_Runtime runtime;
} Image;
*/



