# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright NaN Holding BV. All rights reserved.

bl_info = {
    "name": "AsPtrExample",
    "author": "bd3d, GPT4",
    "version": (1, 0),
    "blender": (3, 4, 0), #.pyd's are made fir 3.4 only!
    "location": "",
    "description": "various example on how to use as_pointer() within .pyds compiled within blender build process",
    "warning": "",
    "category": "",
}

import bpy
import numpy as np

from . import pyds #access ../pyds/readmem.pyd


class ASPTREXAMPLE_PR_Object(bpy.types.PropertyGroup):

    #Example1
    def read_obj_data(self,):
        """pass object adress and access it's properties (open console to view the prints)"""

        obj = self.id_data
        pyds.readmem.read_obj_data(
            obj.as_pointer(),
            )
        
        return None
    
    #Example4
    def render_obj(self,):
        """pass object adress, C++ fct will access obj->mesh->vdata and output a quick render WireFrameRender"""

        # Note: This function could be greatly improved:
        #  - we could pass a directly flatten RGBA value, instead of doing extra np manipulations
        #  - we could pass a numpy array directly instead of a byte_array
        #  - even better, we could pass the image data address directly from our function
                
        obj = self.id_data
        mesh = obj.data

        image = bpy.data.images.get("WireFrameRender")
        if (image is None):
               image = bpy.data.images.new("WireFrameRender", width=1920, height=1080)
        else: image.scale(1920, 1080)
        
        pixels_byte_array = pyds.readmem.render_obj(
            obj.as_pointer(),
            )
        
        # Convert the byte array to a numpy array of uint8 values [0-255]
        pixel_np = np.frombuffer(pixels_byte_array, dtype=np.uint8)

        # Convert the numpy array to a list of floats in the range [0-1]
        pixel_np = (pixel_np.astype(np.float32) / 255.0)

        image.pixels.foreach_set(np.float32(pixel_np).ravel()) #https://devtalk.blender.org/t/bpy-data-images-perf-issues/6459
        image.update()                                         

        return None
    
    
class ASPTREXAMPLE_PR_Mesh(bpy.types.PropertyGroup):

    #Example2
    def read_mesh_elements_mirror(self,):
        """pass mesh verts/edges/polys/loops adresses and their lengths, to read the mesh structure from memory (open console to view the prints)"""

        mesh = self.id_data
        
        print("first 3 verts.co from bpy:")
        for i,v in enumerate(mesh.vertices):
            if (i<3):
                print(" ",i," : ",v.co)
                
        # Using chain of positional arguments: 
        # Verts/Edges/Polygons/Loops [0] adress and their length
        # Advice from Brecht: https://devtalk.blender.org/t/creating-a-mesh-struct-similar-to-blender/28601/9?u=bd3d
        #inconvenient method, but needed if you do not wish to build your .pyd's from blender build process
        
        pyds.readmem.read_mesh_elements_mirror(
            mesh.vertices[0].as_pointer(),
            mesh.edges[0].as_pointer(),
            mesh.polygons[0].as_pointer(),
            mesh.loops[0].as_pointer(),
            len(mesh.vertices),
            len(mesh.edges),
            len(mesh.polygons),
            len(mesh.loops),
            )
        
        return None 
    
    #Example3
    def read_mesh_data(self,):
        """pass mesh data adress and access it's properties and mesh data (open console to view the prints)"""

        mesh = self.id_data
        
        print("first 3 verts.co from bpy:")
        for i,v in enumerate(mesh.vertices):
            if (i<3):
                print(" ",i," : ",v.co)
        
        pyds.readmem.read_mesh_data(
            mesh.as_pointer(),
            )
        
        return None

    #Example5
    def read_mesh_data_mirror(self,):
        """pass mesh data adress and access it's properties and mesh data (open console to view the prints)"""

        mesh = self.id_data
        
        print("first 3 edges vertices idx from bpy:")
        for i,e in enumerate(mesh.edges):
            if (i<3):
                print(" ",i," : ",e.vertices[:])
        
        pyds.readmem.read_mesh_data_mirror(
            mesh.as_pointer(),
            )
        
        return None


def cleanse_modules():
    """remove all plugin modules from sys.modules, will load them again, creating an effective hit-reload soluton
    Not sure why blender is no doing this already whe disabling a plugin..."""
    #https://devtalk.blender.org/t/plugin-hot-reload-by-cleaning-sys-modules/20040

    import sys
    all_modules = sys.modules 
    all_modules = dict(sorted(all_modules.items(),key= lambda x:x[0])) #sort them
    
    for k,v in all_modules.items():
        if k.startswith(__name__):
            del sys.modules[k]

    return None 

def register():

    bpy.utils.register_class(ASPTREXAMPLE_PR_Object)
    bpy.utils.register_class(ASPTREXAMPLE_PR_Mesh)

    bpy.types.Object.asptrex = bpy.props.PointerProperty(type=ASPTREXAMPLE_PR_Object)
    bpy.types.Mesh.asptrex = bpy.props.PointerProperty(type=ASPTREXAMPLE_PR_Mesh)

    return None

def unregister():

    del bpy.types.Mesh.asptrex
    del bpy.types.Object.asptrex

    bpy.utils.unregister_class(ASPTREXAMPLE_PR_Mesh)
    bpy.utils.unregister_class(ASPTREXAMPLE_PR_Object)

    cleanse_modules()

    return None 
