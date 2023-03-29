
bl_info = {
    "name": "PydAsPtr",
    "author": "bd3d, GPT4",
    "version": (1, 0),
    "blender": (3, 40, 0),
    "location": "",
    "description": "read blender memory from as_pointer() within a C++ compiled .pyd, Requirements: WinOS, blender3.40",
    "warning": "",
    "category": "",
}

import bpy

from . import pydll

class PYDASPTR_PR_Object(bpy.types.PropertyGroup):

    def get_object_name(self,):
        obj = self.id_data
        r = pydll.readmem.get_object_name(obj.as_pointer())
        print(r)
        return r

class PYDASPTR_PR_Mesh(bpy.types.PropertyGroup):

    def count_mesh_verts(self,):
        mesh = self.id_data
        r = pydll.readmem.count_mesh_verts(mesh.as_pointer())
        print(r)
        return r

def register():

    bpy.utils.register_class(PYDASPTR_PR_Object)
    bpy.utils.register_class(PYDASPTR_PR_Mesh)

    bpy.types.Object.pydasptr = bpy.props.PointerProperty(type=PYDASPTR_PR_Object)
    bpy.types.Mesh.pydasptr = bpy.props.PointerProperty(type=PYDASPTR_PR_Mesh)

    return None

def unregister():

    del py.types.Mesh.pydasptr
    del bpy.types.Object.pydasptr

    bpy.utils.unregister_class(PYDASPTR_PR_Mesh)
    bpy.utils.unregister_class(PYDASPTR_PR_Object)

    return None 

# Test it via console: 
# >>> C.object.pydasptr.get_object_name()
# >>> C.object.data.pydasptr.count_mesh_verts()