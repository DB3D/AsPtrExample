## **Intro**

This blender plugin demonstrate the use of as_pointer() in Object and Mesh type

**Compatibility:** This project was made in a WinOS environment and built for Blender 3.4  
**Installation:** You will need to build the .pyd or unzip it by yourself in order to test this plugin on your windows machine. (Current .pyd library is heavy, more than 100mb which is not github friendly)

---

## **Implementation**

In order to read blender memory you will need to recreate the matching C++ structs for the memory adress you are passing, and having the required authorization to access blender memory compartimentation. The easiest method to access blender memory is to run your program within the blender program itself, aka from a blender plugin. This leave the following choices:

1.  Read the memory from python Ctypes (here's an [example](https://devtalk.blender.org/t/getting-structure-from-bpy-types-spline-as-pointer-with-ctypes/11759), note that this method is quite slow)
2.  Read the memory via C++ functions using ctypes `.dll` extension
3.  Read the memory via C++ functions using `.pyd` extension, **this is the method we are using in this project** (find a more simpler example on how to extent a blender with .pyd's [here](https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp))

In order to recreate the matching blender structure we have 3 possibilities:

1.  Creating a mirror struct from scratch, ( _Example2_ and _Example5_ is exploring this solution). Note that mirrorring some struct might be challenging, might vary depending on your blender version you'd like to support, and it might not be possible for some more complex struct.
2.  Including blender header in your project.  But you will see quickly that blender C API was not made to be used externally, at best, you'll have a few other dependency files to include, at worst, you'll be digging deep in blender source code rabbit hole to include and link all the needed dependences for the struct you are trying to read. It might lead you nowhere.
3.  Building your .pyd within blender source code directly. Instead of trying to use C API that was never designed to be used in external projects, you can include your whole project within blender source code directly, and tell CMake to build your .pyd when building blender itself. This is the technique we are using on this project on _Example 1,3,4_ , more detail on how this is done in /pyds/source/README.md.

---

## **Examples**

1.  Reading some properties of an object by passing `object.as_pointer()`  as an arg  
    run `C.object.asptrex.read_obj_data()` in blender python console, the results will be printed in the console window
2.  Reading the first 3 vertices coordinates of a mesh data by passing the first adress of each verts/edges/polys/loops elements and their size  
    run `C.object.data.asptrex.read_mesh_elements_mirror()` in blender python console, the results will be printed in the console window
3.  Reading some properties of a mesh and the first 3 vertices coordinates by passing `object.data.as_pointer()` as an arg  
    run `C.object.data.asptrex.read_mesh_data()` in blender python console, the results will be printed in the console window
4.  Reading the object mesh elements by passing `object.as_pointer()`   
    run `C.object.asptrex.render_obj()` in blender python console, there will be a new “WireFrameRender” image in your image editor (see image below)
5.  Same as example2 but by using mirror struct (same technique [Luxcore](https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h) is using)

![Screenshot 2023-03-30 152718](https://user-images.githubusercontent.com/56720730/228869371-422ea721-bdf5-433e-9312-0f7000520a51.jpg)
