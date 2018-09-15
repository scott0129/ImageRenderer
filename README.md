A simple raytracing program that uses a Bounding Volume Hierarchy to speed up performance.

Renders object with a point light source, using the phong model for ambient, diffuse, and specular color coefficients. Creates .obj file renderings using triangular planes, but also capable of rendering spheres and infinite planes.

*adds together pixel values using a sigmoid function rather than flat addition, this was a personal choice, and thought it looked better than linear addition.

usage: ./main <input obj filename> <output png filename> [img Height] [img Width] 
