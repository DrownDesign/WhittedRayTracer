# WhittedRayTracer

Implementing a Whitted Ray-Tracer for an assignment for CGRA408 Victoria University of Wellington.

I will be following along with the follwing scrathapixel tutorial to gain further understanding: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-overview/light-transport-ray-tracing-whitted

To run this program you will need to have the ext folder (./WhittedRayTracer/WhittedRayTracer/ext) included in you external dependencies folder.
1. Go into your .sln.
2. Right click Application in the Solution explorer, Select properties.
3. In directories, select All Configurations, and the in the Include Directories, add the path of the ext folder.

You will also need to set Main.cpp as the startup project.
Then simply run the project in either Release or debug mode. The output image will be located in the solution directory titled
"WhittedOutput".

If you wish to alter the scene, please see the "defineScene" method, and the options struct. Please note the current version only computes intersections for Spheres and Planes (Constructed from triangles).
