# 04 — Delta Time

Goal: your game plays identically at 30, 60 and 144 FPS.

This chapter exists because you have two machines, and this is exactly the bug that
will bite you when you copy the project across. Your Mac runs at 60 or 120 Hz. Your
gaming PC runs at 144 Hz or higher. **The same code will play at different speeds on
each machine** until you fix this — and it will feel like the port is broken.

## C++ you'll meet here

- **`constexpr`** for `FIXED_DT`, `MAX_DELTA` and speeds. Compile-time constants,
  typed, no macro downsides.
- **`std::min` / `std::max`** (`<algorithm>`) — clamping delta. On Windows these are
  the functions that `windows.h`'s `min`/`max` *macros* break, which is why chapter 14
  tells you to `#define NOMINMAX`.
- **Float precision** — `float` gives ~7 significant digits. Accumulating `dt` into a
  running total for minutes on end drifts. Relevant to chapter 07's accumulator; use
  `double` if you're summing elapsed time over a long session.
- **Integer vs float division** — `1 / 60` is `0`, `1.0f / 60.0f` is `0.0167f`. This
  one bites everyone at least once.
- **Units in names** — not a language feature, but the discipline that prevents this
  chapter's entire bug class: `speedPixelsPerSecond` rather than `speed`.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) section 4.

## The bug

In chapter 03 you wrote something like `position.x += speed` where speed is "5".

Five *what*? Five pixels per **frame**. But a frame is not a unit of time. At 60 FPS
that's 300 px/sec. At 144 FPS it's 720 px/sec. Your game is 2.4× faster on the PC.

This is not a small correctness nit. Frame-rate-dependent physics is why old PC
games become unplayable on modern hardware, and why some games' speedruns depend on
capping your framerate.

## The fix

Multiply every per-frame change by the **time that frame took**.

`GetFrameTime()` returns seconds elapsed since the last frame — a `float`, about
`0.0167` at 60 FPS. Now `speed` is in **pixels per second** and multiplying by delta
converts it to "pixels this frame."

Change your speed constant to a per-second value (a few hundred, not five) and let
the multiply do the work. That's the whole change.

**Verify it, don't assume it.** Use `SetTargetFPS` to force 30, then 60, then 144,
and time how long the shape takes to cross the window. If it isn't the same, you
missed a multiply somewhere. Also: `SetTargetFPS(0)` uncaps it entirely — a good
stress test.

## Where it gets subtle: acceleration

Constant velocity is one multiply. Acceleration is two, and the order matters.

```
velocity += acceleration * dt
position += velocity * dt
```

Both lines need `dt`. Miss the first and your acceleration is frame-dependent while
your velocity isn't — a bug that only shows up as "jumping feels different on the
PC," which is maddening to track down.

This exact form (update velocity from acceleration, then position from velocity) is
**semi-implicit Euler**. It's what nearly every 2D game uses. Worth knowing the name:
it's stable enough for games, and it is not the same as updating position first,
which is subtly worse. Try both, watch a bouncing object gain or lose energy.

## The trap: the spiral of death

Delta time alone isn't enough, and here's why.

Drag the window, or let the machine hitch on a background process, and one frame
takes 0.8 seconds. Now `position += velocity * 0.8` teleports your object hundreds
of pixels — straight through a wall your collision check never sampled. Physics
"tunnelling."

Worse: if a slow frame causes work that makes the *next* frame slower, you get a
feedback loop that never recovers. That's the **spiral of death**.

Two standard mitigations:

1. **Clamp delta.** Cap it at, say, 0.1s. The game slows down instead of exploding.
   Two lines, catches most of it, do this now.
2. **Fixed timestep with an accumulator.** Physics steps at a constant `1/60`
   regardless of render rate; you accumulate real elapsed time and run however many
   fixed steps you've earned. Deterministic, and the *correct* answer.

Do the clamp now. **Fixed timestep is chapter 07** — it changes the architecture of
your loop, so it deserves its own chapter and a reason to exist. Note the debt here
and move on.

## Exercises

1. **Measure the bug before fixing it.** With per-frame movement, use `SetTargetFPS`
   to time how long the shape takes to cross the window at 30, then 60, then 144.
   Write the three numbers down. Fix it, repeat, confirm they match.
2. **Integer division.** Print `1 / 60`, `1 / 60.0f`, and `(float)(1 / 60)`. Explain
   the third one — it's the trap, because the cast happens too late.
3. **Acceleration, both ways.** Implement gravity with `dt` applied to both velocity
   and position, then deliberately omit it from the velocity line. Compare jump arcs
   at 30 vs 144 FPS. The second version is subtly wrong in a way that's hard to spot
   without the comparison.
4. **Euler ordering.** Bounce a ball and update position *before* velocity, then
   after. Let each run for a minute. One gains energy, one loses it. Watch it happen.
5. **Simulate a hitch.** Add a key that, when pressed, sleeps for 500ms. Watch your
   object teleport through a wall. Then add the delta clamp and watch it not.
6. **Float drift.** Accumulate `GetFrameTime()` into a `float` and into a `double`
   side by side, print both to 7 decimals, leave it running for a few minutes. Note
   when they diverge.

## Definition of done

- [ ] Every per-frame change is multiplied by `GetFrameTime()`
- [ ] Speeds are expressed per-second, not per-frame
- [ ] I verified identical behaviour at 30 / 60 / 144 FPS
- [ ] Delta is clamped to a sane maximum
- [ ] I can explain why acceleration needs `dt` twice
- [ ] I know what tunnelling is and why chapter 07 exists

Next: [05 — Collision and Your First Real Game](05-collision-first-game.md)
