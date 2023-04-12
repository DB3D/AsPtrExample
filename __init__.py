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
    
#Example6
class ASPTREXAMPLE_OT_write_img_data(bpy.types.Operator):
    
    bl_idname = "asptrex.write_img_data"
    bl_label = "Change image pixels from memory"
    bl_options = {'REGISTER'}
        
    def upd_alpha_strength(self,context):
        
        img = bpy.data.images.get(self.img_name)
        if (not img):
            raise Exception("No Image found")
        
        #on each update, we are re-sending the memory adress to our binaries
        #binary will directly write the new image from memory
        
        pyds.readmem.write_img_data(
            img.as_pointer(),
            self.alpha_strength,
            )
        
        return None
    
    img_name : bpy.props.StringProperty(
        default="",
        )
    
    alpha_strength : bpy.props.FloatProperty(
        name="Alpha Strength",
        default=1.0,
        min=0,
        max=1,
        update=upd_alpha_strength,
        )
    
    def invoke(self, context, event):
        return context.window_manager.invoke_props_dialog(self)
    
    def draw(self, context):
        layout = self.layout
        layout.prop_search(self, "img_name", bpy.data, "images", text="",)
        layout.prop(self, "alpha_strength")
        return None
        
    def execute(self, context):
        return {'FINISHED'}
    
#Example7
class ASPTREXAMPLE_OT_write_mesh_data(bpy.types.Operator):
    
    bl_idname = "asptrex.write_mesh_data"
    bl_label = "Change object mesh from memory (offset verts on Z)"
    bl_options = {'REGISTER'}
    
    zpush : bpy.props.FloatProperty(default=1.0,)

    def execute(self, context):

        obj = context.object
        pyds.readmem.write_mesh_data(   
            obj.as_pointer(),
            self.zpush,
            )

        #update, done via python, couldn't find a way to do it from c++. Looks like we need bContext, and got trouble with passing context correctly
        obj.data.update()
        
        return {'FINISHED'}
    
#Example8
class ASPTREXAMPLE_OT_write_scene_data(bpy.types.Operator):
    
    bl_idname = "asptrex.write_scene_data"
    bl_label = "Attempt to change scene object's locations from memory (impossible?)"
    bl_options = {'REGISTER'}

    def execute(self, context):

        deg = context.evaluated_depsgraph_get()
        
        pyds.readmem.write_scene_data(
            ctx_ptr= context.as_pointer(), 
            deg_ptr= deg.as_pointer(), # pass deg so we can update it
            scn_ptr= context.scene.as_pointer(),
            )
        
        bpy.context.view_layer.update() # ??
        
        return {'FINISHED'}
    
classes = (
    
    ASPTREXAMPLE_PR_Object,
    ASPTREXAMPLE_PR_Mesh,
    ASPTREXAMPLE_OT_write_img_data,
    ASPTREXAMPLE_OT_write_mesh_data,
    ASPTREXAMPLE_OT_write_scene_data,
    
    )

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

    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.Object.asptrex = bpy.props.PointerProperty(type=ASPTREXAMPLE_PR_Object)
    bpy.types.Mesh.asptrex = bpy.props.PointerProperty(type=ASPTREXAMPLE_PR_Mesh)

    return None

def unregister():

    del bpy.types.Mesh.asptrex
    del bpy.types.Object.asptrex

    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    cleanse_modules()

    return None 
