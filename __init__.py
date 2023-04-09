
bl_info = {
    "name": "AsPtrExample",
    "author": "bd3d, GPT4",
    "version": (1, 0),
    "blender": (3, 4, 0), #and 3.4 only!!! will need to quadruple check that our CPP struct are matching exact b3d source code
    "location": "",
    "description": "various example on how to use as_pointer() within a C++ compiled .pyds (Requirements: WinOS, blender3.40)",
    "warning": "",
    "category": "",
}


# Test this plugin via the console: 
# >>> C.object.asptrex.read_obj_data()
# >>> C.object.data.asptrex.read_mesh_data() -> open "MeshReadings" image to verify mesh read
# >>> C.object.data.asptrex.read_mesh_elements() -> open "MeshReadings" image to verify mesh read


import bpy
import numpy as np

from . import pyds #access ../pyds/readmem.pyd


class ASPTREXAMPLE_PR_Object(bpy.types.PropertyGroup):

    def read_obj_data(self,):
        """pass object adress and access it's name"""

        obj = self.id_data
        r = pyds.readmem.read_obj_data(
            obj.as_pointer(),
            )
        return r

class ASPTREXAMPLE_PR_Mesh(bpy.types.PropertyGroup):

    def read_mesh_data(self,):
        """pass mesh data as pointer, LuxCore style, not advised  WIP currently it's crashing"""

        mesh = self.id_data
        r = pyds.readmem.read_mesh_data(
            mesh.as_pointer(),
            )
        return r

    def read_mesh_elements(self,):
        """pass mesh elements first item as_pointer() and their lenght, verify mesh readings with 'MeshReadings' image'"""

        mesh = self.id_data

        # Create a new image or get the existing one

        height,width = 1080,1920 #1080x1920 is non-negotiable for this project
        image = bpy.data.images.get("MeshReadings")
        if (image is None):
               image = bpy.data.images.new("MeshReadings", width=width, height=height)
        else: image.scale(width, height)

        # #Test-Print if we did read the correct information in our C++ module
        # 
        # print("Bpy first 5 Vertices co:")
        # for i,v in enumerate(mesh.vertices):
        #     if i>5: break
        #     print("    ",i," : ",v.co)

        # print("Bpy first 5 Edges vertid:")
        # for i,e in enumerate(mesh.edges):
        #     if i>5: break
        #     print("    ",i," : ",e.vertices[:])

        # print("Bpy first 5 Polygons totloop:")
        # for i,p in enumerate(mesh.polygons):
        #     if i>5: break
        #     print("    ",i," : ",p.loop_total)

        # Using chain of positional arguments: 
        # Verts/Edges/Polygons/Loops [0] adress and their length
        # Advice from Brecht: https://devtalk.blender.org/t/creating-a-mesh-struct-similar-to-blender/28601/9?u=bd3d
        
        pixels_byte_array = pyds.readmem.read_mesh_elements(
            mesh.vertices[0].as_pointer(),
            mesh.edges[0].as_pointer(),
            mesh.polygons[0].as_pointer(),
            mesh.loops[0].as_pointer(),
            len(mesh.vertices),
            len(mesh.edges),
            len(mesh.polygons),
            len(mesh.loops),
            )

        #TODO will need to directly output a flatten RGBA value

        # Convert the byte array to a numpy array of uint8 values [0-255]
        pixel_np = np.frombuffer(pixels_byte_array, dtype=np.uint8)

        # Convert the numpy array to a list of floats in the range [0-1]
        pixel_np = (pixel_np.astype(np.float32) / 255.0)

        image.pixels.foreach_set(np.float32(pixel_np).ravel()) #https://devtalk.blender.org/t/bpy-data-images-perf-issues/6459
        image.update()                                         #TODO: would be even faster if we pass pixels.as_pointer() and directly write in memory

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
