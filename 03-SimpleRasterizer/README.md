# SimpleRasterizer

Extending the <b>[TriangleApp](https://github.com/ForgeCor3/VulkanLearning/tree/master/02-TriangleApp)</b> project, turning it into a 3d model rasterizer, continuing the <b>[Khronos Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html)</b>.</br>
The 3d model is taken from an <b>[open source](https://sketchfab.com/3d-models/pantheon-f87f491c5c7b4160bf925f2466f2c8c6)</b> and slightly edited for correct display.

![03-SimpleRasterizer](https://github.com/ForgeCor3/VulkanLearning/blob/master/images/03-SimpleRasterizer.gif?raw=true)

## How to run

There is code left in the project regarding Vulkan validation layers: <b>[VK_LAYER_KHRONOS_validation](https://vulkan.lunarg.com/doc/view/1.3.204.1/windows/khronos_validation_layer.html).</b>

To run the project having saved validation using validation layers, it is necessary to build it in Debug configuration.
<pre>
> cd 03_SimpleRasterizer/
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Debug
> cd Debug
> .\03_SimpleRasterizer.exe
</pre>

To run the project without validation layers, you need to build it in Release configuration
<pre>
> cd 03_SimpleRasterizer/
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Release
> cd Release
> .\03_SimpleRasterizer.exe
</pre>