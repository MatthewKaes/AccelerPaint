AccelerPaint
=============

A simple open source image manipulation software with OpenCL acceleration. AccelerPaint can load basic image files (PNG, GIF, BMP, JPG). AccelerPaint currently supports layers, scrolling for large images and rendering out multilayered images to a single composite file. A simple interface is currently implemented for loading and managing files however manipulation features are not yet implemented.

Current Interface
![Current Setup](img/Sample.png?raw=true)

Example .JPEG Rendering
![Current Setup](img/render.jpg?raw=true)

Current output rendering uses wxWidgets to blend the images together currently. This leads to mixed results with alpha channels that are only slightly transparent. The OpenCL Alpha Blending Kernel is being written first to handel the issue but there isn't a better working solution just yet for computers where OpenCL is not supported.


Multiplatform
=============

AccelerPaint is built to work on a number of platforms. By using OpenCL to accelerate the rendering and wxWidgets for the form development AccelerPaint can work on Windows, Mac, or Linux operating systems.


Build Instructions
==================

Install wxWidgets and OpenCL

Create an environment variable called OPENCL to the OpenCL location.
