-- premake5.lua
workspace "RockenRaster"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RockenRaster"

filter "action:vs2022"
   toolset "v145"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RockenRaster"