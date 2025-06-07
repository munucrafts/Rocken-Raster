-- premake5.lua
workspace "Rastero"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Rastero"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Rastero"