## Overview

A fully-lit Model Loader made with C++ and OpenGL

![image](https://github.com/user-attachments/assets/5ed1bb92-81a6-4f1c-bc30-a0e3608ec5ce)
![image](https://github.com/user-attachments/assets/4fe6e51a-2857-4b8e-900e-6e981ae8c649)


Made by following an online book from https://learnopengl.com/

## Libraries used
- Glad to load OpenGL functions
- GLFW for window creation and Inputs
- GLM for math
- Assimp for model loading
- stb_image for loading textures
- ImGui for UI

## Steps to build
1. Install CMake
2. Run the following commands in PowerShell/terminal in the folder where CMakeLists.txt is located
```
mkdir build
cd build
cmake ../
```

## Features
- Model Loading with textures using Assimp (Only .obj)
- Phong-lighting
- Specular and Diffuse maps
- Directional lights, Point lights, and spot lights
- Customizable parameters using ImGui
- Free 6-directional movement with zooming
  




