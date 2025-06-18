-- premake5.lua
workspace "RockenRaster"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RockenRaster"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RockenRaster"