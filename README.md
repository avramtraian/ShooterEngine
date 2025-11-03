## ShooterEngine - Overview

ShooterEngine is a **minimalistic** yet extensible 3D game engine written in C++, aimed at giving developers **full
control over performance-critical code** while remaining **lightweight and streamlined**.

Its design philosophy centres on being highly configurable, avoiding heavyweight dependencies, and favouring a **“build
your own game”** approach rather than shipping a full editor out-of-the-box.

## Build Instructions

*Note:* Only Windows is currently supported.

*Note:* When compiling using the *MSVC* toolchain, all operations must be performed from a *Developer Command Prompt*.
Otherwise, the build scripts will skip those configurations.

There are three build types regarding optimization level and enabled internal debugging tools/systems: ***Debug***,
***Development*** and ***Shipping***. Each build type can be compiled using either the ***MSVC*** or ***Clang***
toolchains, and using either the ***Visual Studio*** (which is referred to as ***VS*** in the build system naming
scheme) or ***Ninja*** generators. The final build configuration will thus be named by the following template:
**Type**-**Toolchain**-**Generator**. Configure and build the project by following the next steps:
    
**1.** Ensure the following *prerequisite tools* are installed:
<div style="margin-left: 2em">

- *CMake* (any version after *3.20*).
- Either the *MSVC* toolchain or the *Clang* toolchain with support for *C++20*.
- Either *Visual Studio* or *Ninja* (as CMake generators).
- The *Vulkan SDK*, which includes the *DXC* shader compiler (only version *1.4.321.1* was officially tested).

</div>

**2.** Either configure all possible configurations by executing the **Scripts/ConfigureAll.bat** script or configure a
specific configuration by executing the following command (the exact command can be extracted from the previous script
contents):
<div style="margin-left: 2em">

```shell
cmake -S . -B Build/[Type]-[Toolchain]-[Generator] -G "[Generator]" -DCMAKE_BUILD_TYPE=[Type] -DCMAKE_C_COMPILER=[Toolchain C compiler] -DCMAKE_CXX_COMPILER=[Toolchain C++ compiler]
```

</div>

**3.** Either build all possible configurations (which might take a while) by executing **Scripts/BuildAll.bat** script
or build a specific configuration by executing the following command (the exact command can be extracted from the
previous script contents):
<div style="margin-left: 2em">

```shell
cmake --build Build/[Type]-[Toolchain]-[Generator]
```

*Note:* When build a configuration that uses the *Visual Studio* generator you must also specify the target
configuration by appending to the former command template the following:
```shell
--target [Type]
```

</div>

**4.** All compiled binaries will be placed in their corresponding build directory. You can launch the editor by opening
the **SE-Editor.exe** executable or the standalone game by opening the **SE-Game-Standalone.exe** executable. If the
configuration uses *Visual Studio* as the generator, you can also launch the editor by opening the **ShooterEngine.sln**
solution located in the corresponding build directory, setting **SE-Editor** as the start-up project and pressing
**F5** (same steps for launching the standalone game).

## Features

* Vulkan rendering backend: the engine uses the Vulkan graphics API for rendering.
* Primitive scene system: a lightweight scene graph / scene management layer is in place, enabling you to create scenes,
manage entities, and traverse them.
* Custom serialization architecture: the scene system and other subsystems use a home-designed serialization system to
load/save scene data, assets, etc.
* In-development asset system: groundwork is laid for asset management (importing/loading resources, metadata tracking)
though it’s still being matured.
* Custom home-grown STL library: instead of relying solely on the standard library containers, the project includes a
custom container/utility library (a lightweight “mini-STL”) to reduce dependencies and maximise control.

## What's missing

* Editor: there is no full-fledged visual editor UI yet (level-editing, asset-browser, drag-and-drop, etc.).
* Scene rendering pipeline: while basic rendering via Vulkan is there, the full scene rendering features (lighting,
shadows, post-processing, etc.) are not yet complete.
* Packaging system: support for packaging builds, asset bundling, deployment pipelines and platform installers is not
yet implemented.

## Next Steps

* Develop the editor UI: create a scene editor, asset browser, and real-time preview.
* Expand rendering pipeline: add lighting (PBR), shadows, environment mapping, post-process effects.
* Finalize the asset system: importers for common formats (FBX/GLTF/PNG/…); runtime hot-reload; asset dependency
tracking.
* Implement build/packaging system: allow bundling game data, target multiple platforms, installer generation
versioning.
* Optimize engine subsystems: memory allocators, multithreading of render and asset loading, profiling tools.
