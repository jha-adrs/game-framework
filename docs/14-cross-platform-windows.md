# 14 — Cross-Platform Reality

CMake gets you *compiling* on both machines. It does not get you *behaving the same*
on both. This is a list of the specific things that will differ, roughly in the order
you'll meet them.

## C++ you'll meet here

- **Preprocessor conditionals** — `#if defined(_WIN32)` / `#elif defined(__APPLE__)`.
  Prefer `#if defined(X)` over `#ifdef X`: it composes with `&&` and `!`.
- **Platform macros** — `_WIN32` (defined on 64-bit Windows too, despite the name),
  `__APPLE__`, `__linux__`. Compiler macros: `_MSC_VER`, `__clang__`, `__GNUC__`.
- **Macros are textual and hostile** — `windows.h`'s `min`/`max` macros break
  `std::min`/`std::max` at every call site with an incomprehensible error. `NOMINMAX`
  is the fix. This is the best possible argument for `constexpr` over `#define` in
  your own code.
- **`<cstdint>`** — `int32_t`, `uint8_t`. Use these for anything written to disk or
  sent over a wire, where "however wide `int` is here" isn't good enough.
- **`std::filesystem`** (`<filesystem>`, C++17) — `path`, `exists`, `current_path`.
  Handles separators for you. Note it needed a separate link flag on older toolchains;
  on your two it should just work.
- **`static_assert`** — compile-time checks. `static_assert(sizeof(void*) == 8)` if
  you want to refuse 32-bit builds loudly rather than mysteriously.
- **Per-compiler warning flags in CMake** — `if(MSVC) ... else() ...`. MSVC wants
  `/W4`, clang wants `-Wall -Wextra`. Fixing both compilers' warnings finds real bugs
  neither finds alone.
- **`#pragma warning(push/pop)`** (MSVC) and `#pragma clang diagnostic` — for
  silencing a warning in third-party headers without disabling it project-wide.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 4, 11 and 12.

## The raylib / windows.h name collisions (real, and confusing)

`windows.h` defines symbols that clash with raylib's API. If anything in your build
pulls it in — directly, or via a third-party library — you get bizarre errors on
Windows only, in code that compiles fine on your Mac.

Known collisions: **`Rectangle`**, **`CloseWindow`**, **`ShowCursor`**,
**`LoadImage`**, **`DrawText`**, **`DrawTextEx`**.

Mitigations, in order of preference:

1. **Don't include `windows.h`.** raylib covers everything you need; you rarely have
   a real reason to.
2. If something forces it, define these *before* including it:
   ```cpp
   #define NOGDI      // kills Rectangle, DrawText, and friends
   #define NOUSER     // kills CloseWindow, ShowCursor
   #define NOMINMAX   // kills the min/max MACROS that break std::min/std::max
   ```
   `NOMINMAX` you will want regardless — those macros breaking `std::min` is a
   Windows rite of passage.
3. Include `raylib.h` *after* `windows.h` if you must have both.

## Compilers disagree

MSVC and clang are both conforming, and both accept things the other rejects.

- **Warnings are different.** Turn them up on both and fix what each finds — MSVC
  catches uninitialised-variable cases clang misses, and vice versa. In CMake, guard
  per compiler: `/W4` for MSVC, `-Wall -Wextra` for clang/GCC. Fixing both compilers'
  warnings genuinely finds real bugs.
- **MSVC is stricter about templates instantiated late**, and looser about some
  narrowing conversions.
- **No compiler extensions.** If you accidentally use a GCC/clang extension (VLAs,
  nested functions), MSVC rejects it. Stick to standard C++.
- **`/utf-8`** — MSVC's default source encoding is not UTF-8. If you have non-ASCII
  characters in string literals, pass `/utf-8` or get mojibake.

## Filesystem

- **Case sensitivity.** Windows is case-insensitive. macOS default (APFS) is *also*
  case-insensitive, so you'll get away with `Player.png` vs `player.png` on both —
  and then it breaks the day anything touches Linux, or a case-sensitive volume.
  Be consistent anyway; it costs nothing.
- **Path separators.** `\` vs `/`. Windows accepts `/` in almost all APIs, so just
  always use `/` and stop thinking about it. Use `std::filesystem::path` where you're
  manipulating paths rather than string concatenation.
- **Line endings.** Handled — the `.gitattributes` in this repo normalises to LF in
  the repo and checks out natively. Without that you get whole-file diffs whenever
  you switch machines.

## Working directory — the one that will actually get you

Your program's current working directory is **not** reliably where the executable is.

- macOS, run from terminal → wherever you ran it from.
- Windows, run from Visual Studio → usually the *project* directory.
- Either, double-clicked → varies.
- macOS `.app` bundle → something you did not expect.

So `LoadTexture("assets/player.png")` works in one context and silently returns a
null texture in another. The fix: resolve paths relative to the executable using
`GetApplicationDirectory()`, and/or call `ChangeDirectory()` once at startup. Do it
before you have many assets.

Related: have CMake **copy your `assets/` folder next to the built binary** as part of
the build, so the layout is identical to a shipped build. Look up
`add_custom_command(TARGET ... POST_BUILD ... copy_directory)`. This removes a whole
category of "works on my machine."

## Build config actually matters

Debug builds are genuinely **5–20× slower** than Release for the kind of tight loops
games have. If your game feels sluggish, check what you built before you optimise
anything.

```
cmake --build build --config Release      # Windows (multi-config)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build   # macOS
```

That asymmetry — config at build time on Windows, configure time on macOS — is worth
wrapping in a `build.sh` / `build.bat` pair so you stop mistyping it.

## Linking

raylib's CMake builds a **static** library by default, so there's no DLL to ship
next to your exe. Good — keep it that way. If you ever set `BUILD_SHARED_LIBS ON`,
you inherit the whole "DLL not found" class of problems on Windows.

## Sanity check for this chapter

Commit on one machine, pull on the other, build, run. Anything that differs is a bug
you now know about. Do this **regularly**, not once at the end — a month of drift is
much harder to unpick than a day of it.

## Exercises

1. **One platform branch.** Write a function returning the platform name via
   `#if defined`. Confirm it reports correctly on both machines. Keep it small — the
   goal is one place where platform code lives, not `#ifdef` scattered everywhere.
2. **Feel the macro problem.** On Windows, include `windows.h` before your code and
   call `std::min`. Read the error. Add `NOMINMAX`. Read the difference. This is the
   most confusing error in the chapter and it's worth meeting deliberately.
3. **Warnings, both compilers.** Add per-compiler warning flags to CMakeLists. Build
   clean on both. Write down anything one compiler caught that the other didn't —
   there is usually at least one.
4. **Working directory.** Print `std::filesystem::current_path()` at startup. Run
   from the terminal, from your IDE, and by double-clicking the binary. Three
   different answers. Now fix asset loading with `GetApplicationDirectory` so all
   three work.
5. **Asset copy step.** Make CMake copy `assets/` next to the binary post-build.
   Confirm a fresh `build/` directory produces a runnable layout with no manual steps.
6. **Debug vs Release.** Time your worst frame in both configs. Note the multiple.
   Then remember this number the next time something "feels slow."

## Definition of done

- [ ] Same commit builds warning-clean on MSVC and clang
- [ ] Assets load regardless of working directory
- [ ] CMake copies assets next to the binary
- [ ] Release builds on both platforms
- [ ] `build.sh` and `build.bat` wrap the per-platform incantations
- [ ] I pull and build on the other machine at least weekly
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

Next: [15 — Testing and Profiling](15-testing-and-profiling.md)
