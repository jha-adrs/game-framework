# 01 — Toolchain and Build

Goal: the same `CMakeLists.txt` produces a running raylib program on macOS **and**
Windows, with no package manager on either machine.

## Why CMake, when a one-line compile command would do

On macOS alone you could get away with:

```
clang++ main.cpp -o game -lraylib -framework Cocoa -framework IOKit \
        -framework CoreVideo -framework OpenGL
```

That line is worth understanding once (see "The two-step model" below), but it is
useless on Windows — different compiler, different flags, no `-framework`, no
`brew install raylib`. Maintaining two build scripts means they drift, and you
discover the drift on the machine you're not sitting at.

CMake is a **build-system generator**. You describe your project once; it emits
Makefiles on macOS and a Visual Studio solution on Windows. Combined with
`FetchContent`, it also downloads and compiles raylib itself — so neither machine
needs raylib pre-installed.

The cost is one config file and some new vocabulary. Worth it here.

## The two-step model (the thing to actually understand)

C++ has two distinct phases, and every build error you hit belongs to exactly one
of them. Coming from a managed language this is the biggest adjustment.

**1. Compile.** `#include "raylib.h"` is *literal text pasting* — the preprocessor
copies the header's contents into your file before the compiler sees it. That gives
you **declarations**: "a function `InitWindow(int, int, const char*)` exists." The
compiler now type-checks your calls. It does **not** know what `InitWindow` does.

**2. Link.** The actual machine code for `InitWindow` lives inside `libraylib.a`.
The linker's job is to match every symbol you referenced to a definition somewhere,
and stitch them into one executable.

This is why you need two kinds of flags: `-I` (where to find *headers*, compile
phase) and `-L` / `-l` (where to find *libraries* and which ones, link phase).

**Error triage.** Classify before you debug:

| Error text looks like | Phase | Actual cause |
|---|---|---|
| `'raylib.h' file not found` | compile | include path wrong (`-I`) |
| `unknown type name 'Color'` | compile | header not actually included |
| `use of undeclared identifier 'DrawCircle'` | compile | typo, or wrong header |
| `Undefined symbols: _InitWindow` | **link** | library not linked (`-l`), or not found (`-L`) |
| `ld: library not found for -lraylib` | **link** | library path wrong |
| `LNK2019: unresolved external symbol` | **link** (MSVC) | same as "undefined symbols" |
| `Symbol not found` *at runtime* | load | dylib/DLL missing next to the exe |

Ninety percent of beginner suffering is misreading a link error as a compile error
and editing includes for an hour.

## Prerequisites

### macOS (you already have most of this)
```
clang++    already present via Xcode Command Line Tools
cmake      brew install cmake        <-- you need this
git        already present
```
CMake needs Git because `FetchContent` clones raylib.

### Windows 11
Install **Visual Studio 2022 Community** (free) and check the
**"Desktop development with C++"** workload. That single checkbox gives you:

- MSVC (the compiler)
- The Windows SDK (needed for windowing, exactly like Cocoa on macOS)
- CMake, bundled
- Ninja, bundled

You do not need Visual Studio as your *editor* — use VS Code with the C/C++ and
CMake Tools extensions if you prefer. You need it for the toolchain.

Alternative: MSYS2 + MinGW-w64 gives you g++ and a Unix-like shell. It feels more
familiar coming from macOS, but MSVC is the path of least resistance on Windows and
every raylib issue thread assumes it. Start with MSVC.

## Write `CMakeLists.txt`

Put this at the repo root. **Type it, don't paste it** — then read the annotations
below and make sure you can explain each block. This is the one place I hand you a
file outright, because CMake boilerplate has a lot of frustration value and almost
no learning value.

```cmake
cmake_minimum_required(VERSION 3.20)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Don't build raylib's ~150 example programs — they triple the build time.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp)
target_link_libraries(game PRIVATE raylib)
```

Line by line:

- **`cmake_minimum_required`** — refuses to run on ancient CMake, and also selects
  which CMake behaviours ("policies") are active. Not optional boilerplate.
- **`project(... LANGUAGES CXX)`** — without `LANGUAGES CXX`, CMake also probes for
  a C compiler and takes longer. `CXX` means C++.
- **`CMAKE_CXX_STANDARD 17`** — which C++ standard. `..._REQUIRED ON` makes it a
  hard error rather than a silent downgrade if the compiler can't do it.
- **`CMAKE_EXPORT_COMPILE_COMMANDS ON`** — writes `build/compile_commands.json`.
  This is what gives clangd / VS Code working autocomplete and go-to-definition.
  Turn it on now; debugging "why is intellisense dead" later is miserable.
- **`FetchContent_Declare` / `MakeAvailable`** — clone raylib at tag `5.5` into
  `build/_deps/` and add it to *your* build as a subproject. `GIT_SHALLOW` skips
  its history (~faster clone). Pinning `GIT_TAG` to a version rather than `master`
  is what makes your Windows machine build the same raylib as your Mac.
- **`target_link_libraries(game PRIVATE raylib)`** — the payoff. raylib's own CMake
  config carries its platform requirements with it, so linking this one target
  pulls in Cocoa/IOKit/CoreVideo/OpenGL on macOS and the right Windows SDK libs on
  Windows. You never write `-framework` again. That is the whole reason for CMake.

## Build and run

```
cmake -S . -B build       # configure: read CMakeLists, fetch raylib, generate
cmake --build build       # compile and link
```

`-S .` is the source dir, `-B build` the output dir. Everything generated goes in
`build/`, which is gitignored — you can delete it any time and reconfigure.

The **first** configure takes a couple of minutes (it's compiling all of raylib).
After that it's cached in `build/_deps/` and rebuilds are fast.

Running it differs per platform, and this trips everyone:

```
./build/game                  # macOS  — single-config generator
build\Debug\game.exe          # Windows — MSVC is MULTI-config
```

MSVC generators produce Debug *and* Release slots, so the binary lands in a
subdirectory named after the config. On Windows, `cmake --build build --config Release`
puts it in `build\Release\`. On macOS, config is chosen at configure time
(`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`) and there is no extra folder.
Remembering which machine behaves which way is a real ongoing annoyance; a
`build.sh` / `build.bat` pair that wraps this is a reasonable thing to add later.

## Smoke test before chapter 02

Create `src/main.cpp` containing only `#include "raylib.h"` and an empty `main`
returning 0. If `cmake -S . -B build && cmake --build build` succeeds, your
toolchain is done and every later error is *your* code, not your setup.

Verifying that separation now is worth the extra five minutes.

## Definition of done

- [ ] `brew install cmake` on macOS
- [ ] Visual Studio 2022 + "Desktop development with C++" on Windows
- [ ] `CMakeLists.txt` written and understood line by line
- [ ] Empty program compiles and links on macOS
- [ ] Same commit compiles and links on Windows
- [ ] You can state, without looking, the difference between a compile error and a link error

Next: [02 — Window and Color](02-window-and-color.md)
