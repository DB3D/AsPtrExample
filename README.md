**Intro**

This Blender plugin demonstrates the use of **as_pointer()** with Object and Mesh types in Blender.

**Compatibility:** This project was made in a WinOS environment and built for Blender 3.4.  
**Installation:** You will need to build the .pyd or unzip it by yourself to test this plugin on your Windows machine. (The current .pyd library is large, more than 100MB, which is not GitHub-friendly)

---

## **Implementation**

To read Blender memory, you'll need to recreate the matching C++ structs for the memory addresses you are passing and have the required authorization to access Blender's memory compartmentalization. The easiest way to access Blender memory is to run your program within Blender itself, as a Blender plugin. This leaves the following choices:

1.  Read the memory from Python Ctypes (here's an [example](https://devtalk.blender.org/t/getting-structure-from-bpy-types-spline-as-pointer-with-ctypes/11759), note that this method is quite slow)
2.  Read the memory via C++ functions using ctypes **.dll** extension
3.  Read the memory via C++ functions using **.pyd** extension, **this is the method used in this project** (find a simpler example on how to extend Blender with .pyd's [here](https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp))

In order to recreate the matching blender structure we have 3 possibilities:

1.  Create a mirror struct from scratch (Examples 2 and 5 explore this solution). Note that mirroring some structs might be challenging, might vary depending on the Blender version you'd like to support, and it might not be possible for more complex structs.
2.  Include Blender headers in your project. However, you'll quickly realize that the Blender C API was not designed to be used externally. At best, you'll have a few other dependency files to include; at worst, you'll be digging deep into the Blender source code rabbit hole to include and link all the required dependencies for the struct you are trying to read. It might lead you nowhere.
3.  Build your .pyd within the Blender source code directly. Instead of trying to use the C API that was never designed to be used in external projects, you can include your entire project within the Blender source code and tell CMake to build your .pyd when building Blender itself. This technique is used in this project, and Examples 1, 3, and 4 rely upon it. More details on how this is done can be found in /pyds/source/README.md. ( _Please note that the C API is partially available, as we discovered in Example8, some crutial function to deal with bContext and Depsgraph updates seem to not work properly from our library_ )

**Examples**

1.  Read some properties of an object by passing **object.as_pointer()** as an argument.  
    Run **C.object.asptrex.read_obj_data()** in the Blender Python console; the results will be printed in the console window.
2.  Read the first three vertex coordinates of mesh data by passing the first address of each verts/edges/polys/loops element and their size.  
    Run **C.object.data.asptrex.read_mesh_elements_mirror()** in the Blender Python console; the results will be printed in the console window.
3.  Read some properties of a mesh and the first three vertex coordinates by passing **object.data.as_pointer()** as an argument.  
    Run **C.object.data.asptrex.read_mesh_data()** in the Blender Python console; the results will be printed in the console window.
4.  Read the object mesh elements by passing **object.as_pointer()**.  
    Run **C.object.asptrex.render_obj()** in the Blender Python console; a new "WireFrameRender" image will appear in your image editor (see image below).
5.  Same as Example 2, but using mirror struct (similar technique to what [LuxCore](https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h) is using).
6.  Directly write image data from memory by passing **img.as_pointer()**.  
    Search for **write_img_data** operator to test this example.
7.  Directly write object data from memory by passing **obj.as_pointer()**.  
    Search for **write_mesh_data** operator to test this example.
8.  Change the scene's object's location from memory by passing **scene.as_pointer()**. Additionally, we pass **context.as_pointer()** and **depsgraph.as_pointer()**, needed for some C functions we are trying to use. Unfortunately, this exercise led to a dead end, as we weren't able to successfully send an update message to the depsgraph from our dynamic library. Additionally, we couldn't read the context from memory. It seems that we hit limitations in this exercise. Not everything in the C API is accessible from a library implementation.

![Screenshot 2023-03-30 152718](https://user-images.githubusercontent.com/56720730/228869371-422ea721-bdf5-433e-9312-0f7000520a51.jpg)
