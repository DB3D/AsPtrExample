# AsPtrExample
This plugin demonstrate the use of as_pointer() in Object and Mesh type
Projected created on WindowsOS, using Visual Studio 2017 and blender 3.40 series

This example project consists of 3 example:
Example1: Reading the name from Object struct. We obtained the struct by including the original source code in our IDE
Example2: Reading Mesh.vertices/edges/polygons/loops by looping their arrays, we replicate the simple meshdata struct in the project
Example3: Reading Mesh struct directly, the Mesh struct is harder to reproduce and to link in your project.

revelant files:
- `\\__ini__.py`
- `\\VS17 Project\main.cpp`
- `\\VS17 Project\_read_obj_data.cpp` (example1)
- `\\VS17 Project\_read_mesh_elements.cpp` (example2)
- `\\VS17 Project\_read_mesh_data.cpp` (example3)  

See the comment on each of these files

We are relying on .pyd to read blender memory, see "_FailedAttempt" folder
If you never built your own .pyd before, start with a simpler project, for example:
https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp

------------------------------------------------------------------------------------------------------

TODO `_read_mesh_elements.cpp`:

- Find how to return a numpy array instead of bytearray for the generated pixels
- Find if it is possible to direcyl pass `image.pixels.as_pointer()` within our C++ application and write the render from memory?

TODO `_read_mesh_data.cpp`:

- Resolve crashing issue, accessing Mesh struct is challenging. Coudln't figure that out yet. We are trying to follow the LuxCore method  https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h
