# Spark

Spark is an a graphics engine with performance in mind.

# Prerequisites

- Windows (Currently working only on this os)
- cmake (version >= 3.17)
- MSVC compiler which support cpp17 (currently the only supported compiler - tested on msvc142 as provided by vs2019)
- Vulkan sdk (version >= 1.2) and a gpu+driver which supports it
- git lfs

# Compiling and running

## clone the repo:

```bat
git clone --recurse-submodules https://github.com/NadavT/Spark Spark
cd Spark
```

## build

Open a supported build shell (`x64 Native Tools Command Prompt` for example.)

### using nmake - provided by MSVC by default:

```bat
mkdir install
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" ..
cmake --build . -j 6
cmake --install . --component sandbox
```

### using ninja:

```bat
mkdir install
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
cmake --build . -j 6
cmake --install . --component sandbox
```

## Run

Run `sandbox.exe` from install dir.

### Features:

Moving around:

- Rotating cam using `middle mouse button` and moving the mouse.
- Panning cam using `middle mouse button` + `shift` and moving the mouse.

While object is selected:

- `G` - for moving the object
- `R` - for rotating the object
- `S` - for scaling the object
- `T` - for all the above

For free cam:

- Use `space` or `Run` button to enter.
- Use `space` to stop.
- Use `W,A,S,D` to move around.
- Use mouse to look around.
