# OpenGL Pirates Gulf 🏴‍☠️

[cite_start]A 3D photorealistic marine and island scene built from scratch using C++ and OpenGL[cite: 17]. [cite_start]This project presents a pirate-themed environment complete with ships, islands, houses, cannons, a lighthouse, and a dynamic skydome[cite: 27, 28]. [cite_start]The application demonstrates modern computer graphics techniques including advanced lighting, soft shadows, atmospheric fog, and procedural weather effects[cite: 18].

## ✨ Features

* [cite_start]**Advanced Lighting:** * **Directional Light:** Acts as "moonlight" with adjustable intensity over the entire map[cite: 55].
    * [cite_start]**Point Light:** A localized spotlight stationed at the lighthouse, featuring distance attenuation (constant, linear, and quadratic)[cite: 56].
* [cite_start]**Soft Shadows:** Implemented using Shadow Mapping with a high-resolution 4096x4096 depth map and a 3x3 PCF (Percentage-Closer Filtering) algorithm to create realistic, soft shadow edges[cite: 58].
* [cite_start]**Atmospheric Fog:** A blended fog system combining both distance-based exponential fog and height-based fog (centered around the water level) to create a moody, immersive environment[cite: 60].
* [cite_start]**Procedural Rain:** Animated rain effect generated dynamically in screen-space directly within the fragment shader[cite: 61].
* [cite_start]**Interactive Camera:** Free-roam camera with mouse rotation and keyboard movement to explore the map[cite: 36].
* [cite_start]**Object Manipulation:** Real-time translation, rotation, and scaling of a specific cannon model[cite: 32, 41, 42, 43, 44, 45].
* [cite_start]**Multiple Rendering Modes:** Switch between Smooth Shading, Flat Shading, Wireframe, and Point rendering on the fly[cite: 48, 49, 51, 52, 53].

## 🛠️ Technologies Used

* [cite_start]**C++** [cite: 115]
* [cite_start]**OpenGL** (Core Profile) [cite: 17]
* [cite_start]**GLFW** (Window creation and input handling) [cite: 17]
* [cite_start]**GLEW** (OpenGL extension wrangler) [cite: 17]
* [cite_start]**GLM** (Mathematics library for graphics) [cite: 17]
* [cite_start]**Blender** (Used for creating, editing, and optimizing the 3D models) [cite: 19]

## 🎮 Controls

### Camera Controls
| Key/Input | Action |
| :--- | :--- |
| **W / S / A / D** | [cite_start]Move Forward / Backward / Left / Right [cite: 36] |
| **Mouse** | [cite_start]Rotate camera (Yaw / Pitch) [cite: 36] |
| **Shift** | [cite_start]Sprint (Increases movement speed ~3x) [cite: 37] |
| **ESC** | [cite_start]Toggle Mouse Capture (Free cursor / Lock cursor) [cite: 38] |

### Environment & Lighting Controls
| Key/Input | Action |
| :--- | :--- |
| **UP Arrow** | [cite_start]Increase directional light intensity [cite: 93, 154] |
| **DOWN Arrow** | [cite_start]Decrease directional light intensity [cite: 93, 155] |

### Cannon Manipulation
| Key/Input | Action |
| :--- | :--- |
| **I / K** | [cite_start]Move cannon left / right (X-axis) [cite: 42] |
| **J / L** | [cite_start]Move cannon forward / backward (Z-axis) [cite: 43] |
| **Q / E** | [cite_start]Rotate cannon (Y-axis) [cite: 44] |
| **- / +** | [cite_start]Scale cannon down / up [cite: 45] |

### Rendering Modes
| Key | Action |
| :--- | :--- |
| **1** | [cite_start]Solid + Flat Shading [cite: 49] |
| **2** | [cite_start]Wireframe Mode [cite: 51] |
| **3** | [cite_start]Point/Vertex Mode [cite: 52] |
| **4** | [cite_start]Solid + Smooth Shading [cite: 53] |

## 📸 Screenshots

*(Place your screenshots in a folder named `screenshots` in the root directory of your project, then update the filenames below if necessary)*

![Pirates Gulf Overview](./screenshots/overview.png)
*A general overview of the Pirates Gulf scene.*

![Lighting and Shadows](./screenshots/shadows_lighting.png)
*Showcase of the directional lighting, point light on the lighthouse, and soft shadows.*

![Rain and Fog Effects](./screenshots/rain_fog.png)
*Atmospheric distance/height fog combined with the procedural rain effect.*

![Wireframe Mode](./screenshots/wireframe.png)
*The scene rendered in wireframe mode.*

## 🚀 Future Improvements

* [cite_start]Implementation of a real particle system (billboards) for rain[cite: 175].
* [cite_start]Adding collision detection to prevent the camera from passing through objects[cite: 178].
* [cite_start]Automated animations for the pirate characters and ships[cite: 180].
* [cite_start]More complex implementations for dynamic weather phenomena[cite: 179]. 

## 👨‍💻 Author

[cite_start]**Tudor-Adelin Vancea** [cite: 2]
