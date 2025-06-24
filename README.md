# Rocken Raster - A CPU Rasterizer

<img align="right" alt="Coding" width="200" src="Assets/logo.png">

* **Rocken Raster** is a **CPU-based Software Rasterizer** designed to render **3D scenes**.
* It supports modern rendering techniques, including **Triangle Rasterization**, **Depth Testing**, **Basic Shading**, and more — all built from scratch.
* The **Walnut Framework** is used for basic operations such as **Window Creation**, **UI**, and the **GLM Math Library**.


## 🐉 Features

- Custom **OBJ Mesh** and **Texture Loading**  
- **3D Rasterization** with support for both **Orthographic** and **Perspective projection**  
- Multiple **Culling Techniques**: *Backface Culling*, *Depth Test Culling*, and *Clip Space Culling*  
- **Scene Creation** with multiple objects  
- **Camera Controls**: *Movement*, *Rotation*, *Panning*, *Zooming*, *Camera Speed* (supports both Orthographic and Perspective)  
- **Material system** supporting both *Texture-based* and *Color-based* rendering

## 🖼️ Rendering Pipeline

**Model → World → View → Clip → NDC → UV → Pixel**

## 🧭 Conventions

- **Coordinate Convention**: *Right-Handed*  
   - Left → Right = **+X**  
   - Bottom → Top = **+Y**  
   - Camera → Screen = **–Z**  

- **Triangle Winding Convention**: *Counter Clockwise (CCW)*

## 👽 Future Plans

- Improved **Clip Space Culling** with *Retriangulation* instead of full triangle discard  
- **Physically Based Rendering (PBR)** material system  
- **Ray-traced Shadows**  
- **Volumetric Fog**  
- **Particle System** (e.g., smoke, fire)  
- **DLSS-like Super-resolution** system  
- **GPU Hardware Acceleration**  
- General **Performance Optimization**
