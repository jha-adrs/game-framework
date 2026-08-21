# 15 — Testing and Profiling

## Testing: how do you even test a game?

The honest answer: **you don't test the game, you test the parts that aren't the
game.** Anything that needs a window, a GPU or a human is not unit-testable and
trying is a waste of effort.

What *is* testable, and worth testing:

- Collision math — AABB overlap, resolution, edge and corner cases
- Vector math, if you wrote your own
- The fixed-timestep accumulator (a great test: feed it a known sequence of
  frame times, assert the exact number of steps — chapter 07's determinism is
  precisely what makes this possible)
- State machine transitions
- Input mapping — chapter 09's snapshot is why you can feed synthetic input
- Save/load round-trips
- Level or config parsing

Notice the pattern: **every testable thing is testable because you separated it from
rendering.** Test coverage is a *consequence* of the Part 2 architecture, not a
separate activity. If something is hard to test, that usually means the boundary is
wrong.

### Setup

Use [doctest](https://github.com/doctest/doctest) or
[Catch2](https://github.com/catchorg/Catch2) — both single-header-ish and both drop
in via the same `FetchContent` pattern you already know from chapter 01. doctest
compiles faster; Catch2 has nicer output. Either is fine.

Structurally: put shared logic in a library target, then link **both** your game exe
and your test exe against it. That refactor is itself the valuable part — it forces
you to separate "engine" from "main."

```
add_library(gamelib src/collision.cpp src/input.cpp ...)
add_executable(game src/main.cpp)      target_link_libraries(game PRIVATE gamelib raylib)
add_executable(tests tests/main.cpp)   target_link_libraries(tests PRIVATE gamelib doctest)
```

Then `enable_testing()` and `add_test(...)` so `ctest --test-dir build` runs them.

### What not to bother with

Don't mock the renderer. Don't test that `DrawCircle` was called. Don't chase a
coverage number. A dozen tests on your collision and timestep code is a genuinely
good place to stop.

## Sanitizers — better value than tests, honestly

For C++ specifically, sanitizers catch more real bugs per unit of effort than unit
tests do. On macOS/clang:

```
-fsanitize=address,undefined -fno-omit-frame-pointer
```

AddressSanitizer catches use-after-free, buffer overruns and leaks. UBSan catches
signed overflow, bad shifts, null derefs, misaligned access. Both turn "mysterious
crash three minutes later, somewhere else" into "here is the exact line, and here is
where the memory was freed."

Add a CMake option to enable them, run your game under ASan occasionally. MSVC has
`/fsanitize=address` too. **Run it once now** — you almost certainly have at least
one latent bug, and finding it this way is a formative experience.

## Profiling

Rule zero: **profile a Release build.** Debug-build profiles measure the debug build.

Rule one: **measure before optimising.** Your intuition about what's slow will be
wrong, and this is universal, not a beginner thing.

### Tools

- **macOS** — Xcode Instruments (`xcrun xctrace`, or open Instruments.app). The Time
  Profiler template is what you want. Free, already installed, genuinely excellent.
- **Windows** — Visual Studio's built-in profiler (Debug → Performance Profiler).
  Also very good.
- **[Tracy](https://github.com/wolfpld/tracy)** — a frame profiler built for games.
  You annotate scopes and get a live per-frame timeline. This is the one that matters
  for game work, because games care about *frame time consistency*, not average
  throughput. A single 30ms frame every second is a stutter you can feel; an average
  profiler shows it as a rounding error. Worth the setup cost once you have real
  content.

### What to actually look at

- **Frame time, not FPS.** FPS is a reciprocal, so it compresses exactly the range
  you care about. 60→50 FPS and 20→19 FPS look similar in FPS terms and are wildly
  different in milliseconds.
- **The worst frame, not the average.** Spikes are what players feel.
- Common real culprits at your scale, in rough order: loading assets during gameplay
  instead of at scene load; per-frame allocations; O(n²) collision checks over every
  pair; excessive draw calls from many small textures instead of an atlas.
- **Spatial partitioning** (grid, quadtree) is the standard fix for the O(n²)
  collision case — but only once you've measured that collision is the bottleneck.
  A uniform grid is much simpler than a quadtree and usually enough.

## Definition of done

- [ ] Shared logic lives in a library target linked by both game and tests
- [ ] Collision math and the timestep accumulator have tests
- [ ] `ctest` runs them
- [ ] I have run the game under ASan/UBSan and fixed what it found
- [ ] I have profiled a Release build and know my actual worst frame
- [ ] I optimised one thing based on measurement, not a guess

Next: [16 — Shipping and Retrospective](16-shipping-and-retrospective.md)
