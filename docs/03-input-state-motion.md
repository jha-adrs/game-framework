# 03 — Input, State and Motion

Goal: a shape you move with the keyboard. The real goal: understanding where state
lives.

## The core exercise

Draw a rectangle or circle. Move it with the arrow keys or WASD. That's it.

Functions to find in `raylib.h`: `DrawRectangle`, `DrawCircle`, `DrawRectangleV`,
`IsKeyDown`, `IsKeyPressed`, and the `KeyboardKey` enum (`KEY_W`, `KEY_RIGHT`, …).

## The thing to get right: state lives outside the loop

Your position variables are declared **before** `while`, mutated **inside** it.
Declare them inside and they reset every frame — the shape never moves, and the bug
looks like "input isn't working."

Write it wrong on purpose once. Then say out loud what the loop body actually is:
a function from (old state, input) to (new state, pixels).

## `IsKeyDown` vs `IsKeyPressed` — polling vs events

This distinction matters more than it looks.

- **`IsKeyDown`** — is the key held *right now*? True on every frame it's held.
  For continuous things: movement, holding a trigger, charging a jump.
- **`IsKeyPressed`** — did it transition up→down *this frame*? True exactly once
  per physical press. For discrete things: jump, shoot, toggle pause, menu select.

You have no `onKeyDown` callback here. raylib does not push events at you; you
**poll** the current input state at a moment you choose. That is a deliberate
design that fits the loop model — at the top of each frame you ask "what is true
now?" rather than accumulating a queue of things that happened whenever.

Use `IsKeyDown` for pause and you'll pause/unpause 60 times a second and see a
flickering mess. Do it once so the difference is muscle memory.

## Diagonal movement: your first real bug

Move right → speed 5. Move right *and* up → speed 5 right plus 5 up, which is
5·√2 ≈ 7.07 total. Diagonal movement is 41% faster.

Every beginner ships this bug. Fixing it teaches you the one piece of math games
actually require:

1. Build the input as a **direction vector** — x from left/right keys, y from
   up/down — instead of moving on each axis independently.
2. **Normalise** it: divide by its length so it's always length 1 (or zero).
3. Multiply by speed.

Look for `Vector2` in `raylib.h` — a struct of two `float`s. Then look at
`raymath.h` (ships with raylib, separate header) for `Vector2Normalize`,
`Vector2Add`, `Vector2Scale`, `Vector2Length`.

**Write the normalise yourself first, then switch to `raymath.h`.** You need to
know what it does, and you need the guard: normalising a zero-length vector divides
by zero. What does your code do when no keys are held? Find out before it bites you.

## Screen coordinates

+Y points **down**. Origin is top-left. Pressing "up" *decreases* y.

This is near-universal in 2D graphics and it will still confuse you for a week.
Say it out loud each time until it sticks.

## Push a little further

- Clamp the shape inside the window (`GetScreenWidth`, `GetScreenHeight`). Notice
  you must account for the shape's own size, and that a rectangle's x/y is its
  *corner* while a circle's is its *centre*. That asymmetry causes real bugs later.
- Add a second shape with different keys. Notice the copy-paste. **Do not abstract
  it yet** — just notice, and remember the rule: three times, then extract.
- Print the position with `DrawText` and `TextFormat` (raylib's `sprintf`-alike).
  A permanent on-screen readout of your state is the cheapest debugging tool you
  will ever build, and chapter 13 turns it into a real overlay.

## Definition of done

- [ ] A shape moves with the keyboard
- [ ] Diagonal speed equals straight-line speed
- [ ] I can explain `IsKeyDown` vs `IsKeyPressed` and have used both
- [ ] The shape can't leave the window
- [ ] Position is displayed on screen
- [ ] I know what my normalise does when no keys are held

Next: [04 — Delta Time](04-delta-time.md)
