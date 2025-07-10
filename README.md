# **Rocken Raster - A CPU Rasterizer**

<img align="right" alt="Coding" width="235" src="RockenRaster/Assets/logo.png">

* **Rocken Raster** is a **CPU-based Software Rasterizer** designed to render **3D scenes**.

* It supports modern rendering techniques, including **Triangle Rasterization**, **Depth Testing**, **Culling**, **Projections**, **Basic Shading**, and more — all built from scratch.

* The **Walnut Framework** is used for basic operations such as **Window Creation**, **UI**, and the **GLM Math Library**.

<br>

## 🐉 **Available Features**

- Custom **OBJ Mesh** and **Texture Loading**  
- **3D Rasterization** with support for both **Orthographic** and **Perspective projection**  
- Multiple **Culling Techniques**: **Backface Culling**, **Depth Test Culling**, and **Clip Space Culling**  
- **Scene Creation** with **multiple objects**  
- **Camera Controls**: **Movement**, **Rotation**, **Panning**, **Zooming**, **Camera Speed** (supports both **Orthographic** and **Perspective**)  
- Different **View Modes** such as **Lit**, **Unlit**, **Triangulate**, **Depth**, and **Normal**  
- **Material System** supporting both **Texture-based** and **Color-based** rendering  
- Concept of **Static** and **Movable** objects  
- Automatic **View & Transform Baking** for **Camera** and **Entities** for **Optimisations**  
- **Exponential Fog** entity that blends with the **Sky Color** based on **Distance** and **Fog Falloff** value

## 🛩️ **Currently Working On**

- **Physically Based Rendering (PBR)** material system  
- **Ray-traced Shadows**  
- **DLSS-like Super-resolution** system  

## 👽 **Future Plans**

- Improved **Clip Space Culling** with **Retriangulation** instead of full triangle discard  
- **CPU based Particle System**  
- General **Performance Optimization**

## 🖼️ **Rendering Pipeline**

**Model → World → View → Clip → NDC → UV → Pixel**

## 🧭 **Conventions**

- **Coordinate Convention**: **Right-Handed**  
   - Left → Right = **+X**  
   - Bottom → Top = **+Y**  
   - Camera → Screen = **–Z**

- **Triangle Winding Convention**: **Counter Clockwise (CCW)**
