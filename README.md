# GLSL Check

*Because you can't just ship shaders as part of your exe in $CURRENT_YEAR!*

Utility program for catching compile errors of OpenGL shaders. Intended to be part of
a build pipeline or CI.

## Usage

```
glsl-check.exe --help
glsl-check.exe -v c:/mycoolgame/shaders/lighting.vert -f c:/mycoolgame/shaders/lighting.frag
```

## Compilation

[vcpkg](https://learn.microsoft.com/nl-nl/vcpkg/get_started/overview) needs to be installed.

Inside *Developer Command Prompt for VS 2022*:

```console
cmake --preset windows -DCMAKE_BUILD_TYPE=Release
msbuild build\glsl-check.sln /property:Configuration=Release
```

The exe will be located at `build\Release\glsl-check.exe`

Add its containing folder to your PATH environment variable.
