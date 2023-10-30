# TriangleApp

This is my first experience with the Vulkan API and basically working with a low-level graphics API</br>
Project was created according to the [Khronos Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html).

The main goal of the project is to familiarize myself with basic mechanisms and techniques of working with Vulkan and to create a kind of Hello World for graphic programmers.

![02-TriangleApp](https://github.com/ForgeCor3/VulkanLearning/blob/master/images/02-TriangleApp.jpg?raw=true)

## How to run

There is code left in the project regarding Vulkan validation layers: <b>[VK_LAYER_KHRONOS_validation](https://vulkan.lunarg.com/doc/view/1.3.204.1/windows/khronos_validation_layer.html).</b>

To run the project having saved validation using validation layers, it is necessary to build it in Debug configuration.
<pre>
> cd 02-TriangleApp/
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Debug
> cd Debug
> .\02_TriangleApp.exe
</pre>

To run the project without validation layers, you need to build it in Release configuration
<pre>
> cd 02-TriangleApp/
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Release
> cd Release
> .\02_TriangleApp.exe
</pre>