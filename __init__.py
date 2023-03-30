
bl_info = {
    "name": "PydAsPtr",
    "author": "bd3d, GPT4",
    "version": (1, 0),
    "blender": (3, 4, 0),
    "location": "",
    "description": "various example on how to use as_pointer() within a C++ compiled .pyd (Requirements: WinOS, blender3.40)",
    "warning": "",
    "category": "",
}


# Test this plugin via the console: 
# >>> C.object.pydasptr.read_obj_data()
# >>> C.object.data.pydasptr.read_mesh_data()
# >>> C.object.data.pydasptr.render_mesh() -> & open "AsPointerRender" image


#easy quick reload. You will need to shut down blender, no hotreload implemented in this project
if True: 
    import sys, shutil, os
    plug_path = os.path.dirname(__file__) #.../scripts/addons/PydAsPtr/
    pyd_path = os.path.join(plug_path,"pydll","readmem.pyd")
    dll_path = os.path.join(plug_path,"VS17 Project","x64","Release","ReadMemProject.dll")
    if os.path.exists(dll_path):
        if os.path.exists(pyd_path):
            os.chmod(pyd_path, 0o777)  # make the file writable
            os.remove(pyd_path)
        shutil.copyfile(dll_path, pyd_path)



import bpy
import numpy as np

from . import pydll #access ../pydll/readmem.pyd


class PYDASPTR_PR_Object(bpy.types.PropertyGroup):

    def read_obj_data(self,):
        """pass object adress and access it's name"""

        obj = self.id_data
        r = pydll.readmem.read_obj_data(
            obj.as_pointer(),
            )
        return r

class PYDASPTR_PR_Mesh(bpy.types.PropertyGroup):

    def read_mesh_data(self,):
        """pass mesh data as pointer, LuxCore style, not advised  WIP currently it's crashing"""

        mesh = self.id_data
        r = pydll.readmem.read_mesh_data(
            mesh.as_pointer(),
            )
        return r

    def render_mesh(self,):
        """pass mesh elements first item as_pointer() and their lenght, process the mesh into another Mesh struct, and render this mesh in a new image 'AsPointerRender'"""

        mesh = self.id_data

        # Create a new image or get the existing one

        height,width = 1080,1920 #1080x1920 is non-negotiable for this project
        image = bpy.data.images.get("AsPointerRender")
        if (image is None):
               image = bpy.data.images.new("AsPointerRender", width=width, height=height)
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
        
        pixels_byte_array = pydll.readmem.read_mesh_elements(
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

def register():

    bpy.utils.register_class(PYDASPTR_PR_Object)
    bpy.utils.register_class(PYDASPTR_PR_Mesh)

    bpy.types.Object.pydasptr = bpy.props.PointerProperty(type=PYDASPTR_PR_Object)
    bpy.types.Mesh.pydasptr = bpy.props.PointerProperty(type=PYDASPTR_PR_Mesh)

    return None

def unregister():

    del bpy.types.Mesh.pydasptr
    del bpy.types.Object.pydasptr

    bpy.utils.unregister_class(PYDASPTR_PR_Mesh)
    bpy.utils.unregister_class(PYDASPTR_PR_Object)

    return None 
