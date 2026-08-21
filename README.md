# game-framework

Learning C++ and [raylib](https://github.com/raysan5/raylib) from zero, in the open.
No engine yet, no plan to fake one. Just a window, then a shape, then a game, then
whatever I've earned the right to abstract.

**Runs on:** macOS (Apple Silicon) and Windows 11.

## The roadmap

Everything lives in [`docs/`](docs/), numbered in the order it should be done.
Start at [`docs/00-start-here.md`](docs/00-start-here.md).

Each doc gates on the previous one **actually working on both machines**. The later
chapters are deliberately thin — they describe the problem to solve and the questions
to answer, not the API to write. Those decisions need real code in front of them.

C++ itself is taught alongside, not upfront: [`01a`](docs/01a-cpp-refresher.md) is a
refresher for someone who's forgotten the details, and every chapter after it opens
with **C++ you'll meet here** — the language features that chapter makes worth
learning — and closes with **Exercises**, several of which deliberately break
something so the failure is visible rather than theoretical.

## Reading the docs in a browser

```
node tools/docs-server.js
```

Then open <http://localhost:4321>. Zero dependencies — plain Node stdlib plus a
vendored copy of `marked`. Same command on Windows.

## Building the game

Once `CMakeLists.txt` exists (chapter 01 walks you through writing it):

```
cmake -S . -B build          # configure, downloads + builds raylib the first time
cmake --build build          # compile
./build/game                 # macOS
build\Debug\game.exe         # Windows
```

## Rules I set for myself

1. No abstraction until the same code has been written **three** times.
2. Nothing gets committed that I can't explain line by line.
3. Every chapter ends with something runnable, not something architected.
