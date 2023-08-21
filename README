# Description

3D video game, using [Vulkan](https://www.vulkan.org/)/[GLFW](https://www.glfw.org/) for user grapchic.

# Requerements

1. Conan 2.x (install from python with pip: `pip install conan`

2. [`Cmake`](https://cmake.org/download/) *3.5* or later

3. [`Clang`](https://releases.llvm.org/download.html) *16.x* or later

4. [`Ninja`](https://github.com/ninja-build/ninja/releases/latest) generanor (remember for add unzipped dirrectory to PATH (or etc for linux) variable)

# Prebuild

- Install all requerement libraries for project. Just do:
  1. `cd path/to/project/folder`
  2. `conan install . -of=build -b=missing -pr=std20 -s build_type=Debug -s compiler.cppstd=20 -s compiler=clang -s compiler.version=16 --conf tools.cmake.cmaketoolchain:generator=Ninja`
- start configure with: `cmake build --preset <preset>`
 as `<preset>` you can set:
  1. Debug
  2. Release
  3. RelWithDebInfo
  4. MinSizeRel

# Build

 for build use: `cmake --build build --preset <preset> --target all`
 as `<preset>` you can set:

1. Build
2. CleanBuild

# Contributors

- [SummieDaemonium](https://github.com/summiedaemonium)
- [NoneType4Name.](https://github.com/NoneType4Name)
