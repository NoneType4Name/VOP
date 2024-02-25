# Description

Vulkan Objective Project is a wrapper for default [Vulkan](https://www.vulkan.org/) API. Provides Vulkan object classes that don't require explicit allocation and cleanup.

# Requerements

1. Conan *2.x* (install from python with pip: `pip install conan`)

2. [`Cmake`](https://cmake.org/download/) *3.5* or later

3. [`Clang`](https://releases.llvm.org/download.html) *16.x* or later

4. [`Ninja`](https://github.com/ninja-build/ninja/releases/latest) generanor (remember for add unzipped dirrectory to PATH (or etc for linux) variable)

# Install dependencies

- To install all required dependencies for project just do:
  1. `cd path/to/project/folder`
  2. `conan install . -of=bin/build -b=missing -s build_type=Debug -s compiler.cppstd=20 -s compiler=clang -s compiler.version=16 --conf tools.cmake.cmaketoolchain:generator=Ninja`
  3. `conan install ./example -of=bin/build -b=missing -s build_type=Debug -s compiler.cppstd=20 -s compiler=clang -s compiler.version=16 --conf tools.cmake.cmaketoolchain:generator=Ninja`
  **Remark: in conan profile you need to remove compiler.runtime**.

# Configure project

- Start configure with: `cmake --preset conan-debug` and add defenitions use flag -D (to install `BUILD_SHARED_LIBS` call `cmake --preset conan-debug -DBUILD_SHARED_LIBS=ON`)

# Build

- For build use: `cmake --build bin/build --target all`

# Contributors

- [SummieDaemonium](https://github.com/summiedaemonium)
- [NoneType4Name.](https://github.com/NoneType4Name)
