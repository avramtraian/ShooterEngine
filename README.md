## ShooterEngine - Overview

ShooterEngine is a **minimalistic** yet extensible 3D game engine written in C++, aimed at giving developers **full control over performance-critical code** while remaining **lightweight and streamlined**.

Its design philosophy centres on being highly configurable, avoiding heavyweight dependencies, and favouring a **“build your own game”** approach rather than shipping a full editor out-of-the-box.

## Build Instructions

*Note*: Only Windows is currently supported.

**1.** Ensure the following *prerequisite tools* are installed:
<div style="margin-left: 2em">

- *Visual Studio* (only version *17.14.15* was officially tested).
- The *Vulkan SDK* (only version *1.4.321.1* was officially tested).

</div>

**2.** Execute the ***GenerateProjectFiles.bat*** batch script located in the root directory.

**3.** Open the generated ***ShooterEngine.sln*** Visual Studio solution.

**4.** Configure the solution such that ***SE-Editor*** is set as the start-up project and the build configuration is set to ***EditorDevelopment***.

**5.** Launch the editor by pressing ***F5***.

## Features

* Vulkan rendering backend: the engine uses the Vulkan graphics API for rendering.

* Primitive scene system: a lightweight scene graph / scene management layer is in place, enabling you to create scenes, manage entities, and traverse them.

* Custom serialization architecture: the scene system and other subsystems use a home-designed serialization system to load/save scene data, assets, etc.

* In-development asset system: groundwork is laid for asset management (importing/loading resources, metadata tracking) though it’s still being matured.

* Custom home-grown STL library: instead of relying solely on the standard library containers, the project includes a custom container/utility library (a lightweight “mini-STL”) to reduce dependencies and maximise control.

## What's missing

* Editor: there is no full-fledged visual editor UI yet (level-editing, asset-browser, drag-and-drop, etc).

* Scene rendering pipeline: while basic rendering via Vulkan is there, the full scene rendering features (lighting, shadows, post-processing, etc) are not yet complete.

* Packaging system: support for packaging builds, asset bundling, deployment pipelines and platform installers is not yet implemented.

## Next Steps

* Develop the editor UI: create a scene editor, asset browser, and real-time preview.

* Expand rendering pipeline: add lighting (PBR), shadows, environment mapping, post-process effects.

* Finalize the asset system: importers for common formats (FBX/GLTF/PNG/…); runtime hot-reload; asset dependency tracking.

* Implement build/packaging system: allow bundling game data, target multiple platforms, installer generation, versioning.

* Optimize engine subsystems: memory allocators, multithreading of render and asset loading, profiling tools.
