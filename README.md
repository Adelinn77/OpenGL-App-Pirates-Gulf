# OpenGL Pirates Gulf 🏴‍☠️

A 3D photorealistic marine and island scene built from scratch using C++ and OpenGL. This project presents a pirate-themed environment complete with ships, islands, houses, cannons, a lighthouse, and a dynamic skydome. The application demonstrates modern computer graphics techniques including advanced lighting, soft shadows, atmospheric fog, and procedural weather effects.

## ✨ Features

* **Advanced Lighting:** * **Directional Light:** Acts as "moonlight" with adjustable intensity over the entire map.
    * **Point Light:** A localized spotlight stationed at the lighthouse, featuring distance attenuation (constant, linear, and quadratic).
* **Soft Shadows:** Implemented using Shadow Mapping with a high-resolution 4096x4096 depth map and a 3x3 PCF (Percentage-Closer Filtering) algorithm to create realistic, soft shadow edges.
* **Atmospheric Fog:** A blended fog system combining both distance-based exponential fog and height-based fog (centered around the water level) to create a moody, immersive environment.
* **Procedural Rain:** Animated rain effect generated dynamically in screen-space directly within the fragment shader.
* **Interactive Camera:** Free-roam camera with mouse rotation and keyboard movement to explore the map.
* **Object Manipulation:** Real-time translation, rotation, and scaling of a specific cannon model.
* **Multiple Rendering Modes:** Switch between Smooth Shading, Flat Shading, Wireframe, and Point rendering on the fly.

## 🛠️ Technologies Used

* **C++**
* **OpenGL** (Core Profile)
* **GLFW** (Window creation and input handling)
* **GLEW** (OpenGL extension wrangler)
* **GLM** (Mathematics library for graphics)
* **Blender** (Used for creating, editing, and optimizing the 3D models)

## 🎮 Controls

### Camera Controls
| Key/Input | Action |
| :--- | :--- |
| **W / S / A / D** | Move Forward / Backward / Left / Right |
| **Mouse** | Rotate camera (Yaw / Pitch) |
| **Shift** | Sprint (Increases movement speed ~3x) |
| **ESC** | Toggle Mouse Capture (Free cursor / Lock cursor) |

### Environment & Lighting Controls
| Key/Input | Action |
| :--- | :--- |
| **UP Arrow** | Increase directional light intensity |
| **DOWN Arrow** | Decrease directional light intensity |

### Cannon Manipulation
| Key/Input | Action |
| :--- | :--- |
| **I / K** | Move cannon left / right (X-axis) |
| **J / L** | Move cannon forward / backward (Z-axis) |
| **Q / E** | Rotate cannon (Y-axis) |
| **- / +** | Scale cannon down / up |

### Rendering Modes
| Key | Action |
| :--- | :--- |
| **1** | Solid + Flat Shading |
| **2** | Wireframe Mode |
| **3** | Point/Vertex Mode |
| **4** | Solid + Smooth Shading |

## 📸 Screenshots

*(Place your screenshots in a folder named `screenshots` in the root directory of your project, then update the filenames below if necessary)*

![Pirates Gulf Overview](./screenshots/overview.png)

![Lighting and Shadows](./screenshots/shadows_lighting.png)

![Rain and Fog Effects](./screenshots/rain_fog.png)

![Wireframe Mode](./screenshots/wireframe.png)

## 🚀 Future Improvements

* Implementation of a real particle system (billboards) for rain.
* Adding collision detection to prevent the camera from passing through objects.
* Automated animations for the pirate characters and ships.
* More complex implementations for dynamic weather phenomena. 

## 👨‍💻 Author

**Adelinn77**
