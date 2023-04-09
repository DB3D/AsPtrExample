
# development helper, easier to update our .pyd from a fresh compilation
if True:
    
    import os, shutil
    
    # locate our blender library build output 
    dll_path = "C:/Blender/builds/3.4/build_windows_x64_vc17_Release/bin/Release/bakedpyd_AsPtrExample/readmem.dll"
    pyd_path = os.path.join(os.path.dirname(__file__),"readmem.pyd")
    
    # if exists, we update our libs to latest version
    if os.path.exists(dll_path):
        
        # remove existing libs first, if exists
        if os.path.exists(pyd_path):
            os.chmod(pyd_path, 0o777)  # make the file writable, just in case
            os.remove(pyd_path)
            
        # update with latest file
        shutil.copyfile(dll_path, pyd_path)
        

from . import readmem
