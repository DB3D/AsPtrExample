/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright NaN Holding BV. All rights reserved. */

// Example8: Access & write scene data from memory
//           it's a fail! it seems that we cannot change blender memory that easily from a .dll ?
//           conclusion of this exercise, there are some areas we will not be able to modify, object properties is one of them

#pragma once

#include <Python.h>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "BKE_context.h" // bContext
#include "DEG_depsgraph.h" // struct Depsgraph, DEG_id_tag_update
#include "DNA_scene_types.h" // struct Scene
#include "DNA_collection_types.h" // struct Collection
#include "DNA_object_types.h" // struct Object
#include "BKE_object.h" // BKE_object_where_is_calc
#include "WM_api.h" // WM_event_add_notifier

// Main function:

static PyObject* write_scene_data(PyObject* self, PyObject* args, PyObject* kwargs) {
    unsigned long long ctx_ptr;
    unsigned long long deg_ptr;
    unsigned long long scn_ptr;

    // check if positional args & named args are correct
    static const char *kwlist[] = {"ctx_ptr", "deg_ptr","scn_ptr", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "KKK", (char **)kwlist, &ctx_ptr, &deg_ptr, &scn_ptr)) {
        return nullptr;
    };
    
    std::cout << "running write_scene_data in C++" << std::endl;

    // interpret memory
    uintptr_t address_ctx = static_cast<uintptr_t>(ctx_ptr);
    bContext* C = reinterpret_cast<bContext*>(address_ctx);

    uintptr_t address_deg = static_cast<uintptr_t>(deg_ptr);
    Depsgraph* deg = reinterpret_cast<Depsgraph*>(address_deg);

    uintptr_t address_scn = static_cast<uintptr_t>(scn_ptr);
    Scene* scn = reinterpret_cast<Scene*>(address_scn);

    std::cout << " scn->id.name : " << scn->id.name << std::endl;

    Collection *master_collection = scn->master_collection;
    for (CollectionObject *link = (CollectionObject *)master_collection->gobject.first; link; link = link->next) {
        Object *obj = link->ob;
        std::cout << " scn->mastercollection->gobject->id.name: " << obj->id.name + 2 << std::endl;
        
        obj->loc[2] = 0;
        obj->dloc[2] = 0;
        std::cout << " scn->mastercollection->gobject->loc[2] =0"<< std::endl;

        std::fill_n(obj->rot, 3, 0);
        std::fill_n(obj->drot, 3, 0);
        std::cout << " scn->mastercollection->gobject->rot = (0,0,0)"<< std::endl;

        // Send update signal, somehow ?? 
        BKE_object_to_mat4(obj, obj->object_to_world);
        BKE_object_where_is_calc(deg, scn, obj);
        DEG_id_tag_update(&obj->id, ID_RECALC_TRANSFORM);
        WM_event_add_notifier(C, NC_OBJECT | ND_TRANSFORM, NULL);

    }

    //return None, this function just print in console
    Py_RETURN_NONE;
};