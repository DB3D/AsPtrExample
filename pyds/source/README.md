# **Build Instructions**

**How to implement the project within blender source code directly?**

1.  First, build your version of blender, before starting the implementation, here building v3.4. If you are unsure how to build an older version of blender, you can use the following commands, assuming you already built the latest version of blender:   
    `..\blender >>> git checkout blender-v3.4-release`  
    `..\blender >>> git checkout -b my_blender_3.4.0_branch`  
    `..\blender >>> git submodule update --init --recursive`  
    `..\lib\win64_vc15 >>> svn checkout` [`https://svn.blender.org/svnroot/bf-blender/tags/blender-3.4-release/lib/win64_vc15/`](https://svn.blender.org/svnroot/bf-blender/tags/blender-3.3-release/lib/win64_vc15/)  
    `..\blender >>> make`
2.  Create a new folder containing the project source code in a directory of your choice. in the context of this project i advise you do use `blender\source\source\makepyd_MyProject\`
3.  Tell cmake to include your project within blender build process, to do so, add the following line of code  
    `add_subdirectory(makepyd_MyProject)` at the end of `blender\source\source\CMakeLists.txt`
4.  Add the Cmake instructions in your project folder, in our case, the files above `CMakeLists.txt`, `buildinfo.c`, and `buildinfo.h` ( _note that buildinfo's seems to be needed in order to build projects which include any BKE struct._ )
5.  Once built, your libraries will appear on a new folder defined from your `CMakeLists.txt`, Just replace the `.dll` extension with `.pyd` within the context of this project, then proceed to place it in `\pyds\readmem.pyd` and you'll be good to go.

**Implicit Limitations:**

- You'll need to build a .pyd for each blender version your plugin will support.
- You'll need to build a .pyd for each OS's you want to support
