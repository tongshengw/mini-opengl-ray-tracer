# Mini OpenGL ray tracer

This is a C++ OpenGl ray tracer! This project is currently a work in progress.

This project utilises a quaternion based rotation system and uses homogenous coordinate based matrix transformations to manage camera movements. The renderer is created using a screen quad and the fragment shader in OpenGL. Window and keyboard management is done in SDL2.

### Implemented features
- Full camera rotation and translation
- Transferring scene data through uniform variables
- GPU RNG, ray bouncing algorithm
- Sphere primitive

### Future features
- Light transport algorithms for realistic rendering
- Anti aliasing
- Emissive materials
- Subsurface scattering
- Triangle primitives
- Transfering scene data through buffers

The following is a demo where spheres are coloured based on their normal vector.

![](https://github.com/tongshengw/mini-opengl-ray-tracer/blob/main/gifs/raytracer_demo_18012025.gif)
