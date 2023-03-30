# AsPtrExample
This blender plugin demonstrate the use of as_pointer() in Object and Mesh type
Projected created on WindowsOS, using Visual Studio 2017 and blender 3.40 series. 
Curent compiled python libraries `\\pydll\readmem.pyd` are for WinOS only.
Whole has also been uploaded, available within `\\VS17 Project\`

This example project consists of 3 example:
Example1: Reading the name from Object struct. We obtained the struct by including the original source code in our IDE
Example2: Reading Mesh.vertices/edges/polygons/loops by looping their arrays within memory, we replicate the simple meshdata struct of blender, and render a quick image that we load back in an new image data called "AsPointerRender". See image below
Example3: Reading Mesh struct directly, the Mesh struct is harder to reproduce and to link in your project.

![Screenshot 2023-03-30 152718](https://user-images.githubusercontent.com/56720730/228869371-422ea721-bdf5-433e-9312-0f7000520a51.jpg)

revelant files:
- `\\__ini__.py`
- `\\VS17 Project\main.cpp`
- `\\VS17 Project\_read_obj_data.cpp` (example1)
- `\\VS17 Project\_read_mesh_elements.cpp` (example2)
- `\\VS17 Project\_read_mesh_data.cpp` (example3)  

See the comment on each of these files

Prerequisite:
We are relying on .pyd to read blender memory, looks like we don't have much choice (see ".FailedAttempt" folder where i tried to read mem from external process, failed due to memory compartmentalization i bet) If you never built your own .pyd before, start with a simpler project, for example:
https://github.com/DB3D/BpyAddonWithCppModule/blob/main/PydTest/pyd/Project1/add.cpp

------------------------------------------------------------------------------------------------------

TODO `_read_mesh_elements.cpp`:

- Find how to return a numpy array instead of bytearray for the generated pixels
- Find if it is possible to direcyl pass `image.pixels.as_pointer()` within our C++ application and write the render from memory?
- Implement all these finding in a new "SimpleRenderEngine" Github project

TODO `_read_mesh_data.cpp`:

- Resolve crashing issue, accessing Mesh struct is challenging. Coudln't figure that out yet. We are trying to follow the LuxCore method  https://github.com/LuxCoreRender/LuxCore/blob/master/include/luxcore/pyluxcore/blender_types.h



