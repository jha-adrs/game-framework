# 07 — Loop Architecture

**The problem:** chapter 04 left you with variable-timestep physics. It's
non-deterministic (the same inputs produce different results on different machines),
it tunnels through walls on a hitch, and it makes networking or replays impossible.
Your Mac and your 144 Hz PC will not agree.

**The fix:** decouple simulation rate from render rate.

## C++ you'll meet here

- **`constexpr`** — `FIXED_DT`, `MAX_STEPS`. Fixed by definition; say so in the type
  system.
- **Function decomposition** — `update(float dt)` and `render()` as real functions
  with real signatures. What you pass to each tells you a lot about whether your
  state is well-organised.
- **State by reference vs member** — as the loop splits, "where does the game state
  live?" becomes unavoidable. A `GameState` struct passed by reference is the simplest
  honest answer; globals are the tempting wrong one.
- **`const` on `render`** — if rendering takes state as `const&`, the compiler
  enforces that drawing can't mutate simulation. That's a genuinely valuable
  invariant, cheaply obtained.
- **`double` for accumulated time** — see chapter 04's precision note. The
  accumulator itself is fine as `float`; a total-elapsed-seconds counter isn't.
- **`std::chrono`** — worth *knowing* exists (`steady_clock`, `duration`) even though
  `GetFrameTime()` covers you here. It's the standard answer outside raylib.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 2, 3 and 8.

## The idea

Physics runs at a **fixed** step — say exactly 1/60s — no matter how fast you
render. Rendering runs as fast as the display allows.

Each frame you add real elapsed time to an accumulator, then run as many fixed steps
as you've banked, leaving the remainder for next frame:

```
accumulator += min(GetFrameTime(), MAX_DELTA)
while (accumulator >= FIXED_DT) {
    update(FIXED_DT)
    accumulator -= FIXED_DT
}
render()
```

Note that `update` now takes a **constant** — always the same number. That constant
is what buys you determinism.

## Decisions this forces on you

- **What rate?** 60 Hz is the default. 120 Hz costs CPU but reduces tunnelling.
- **Max steps per frame.** Without a cap you've rebuilt the spiral of death inside
  the while loop. Cap it (4–5) and accept that a very slow machine runs in slow
  motion rather than freezing. This is a deliberate trade — name it.
- **Interpolation.** With fixed updates and faster rendering, you'll render *between*
  simulation states, which shows as subtle stutter. The fix is to keep previous and
  current positions and lerp by `accumulator / FIXED_DT` at draw time. This is real
  and worth understanding — but **do it only when you can see the stutter.** It
  doubles your position bookkeeping.
- **Where does input polling go?** Not obvious. Poll once per *frame*, or once per
  *fixed step*? Both are defensible and they behave differently for a single-frame
  tap. Think it through and write down which you chose and why.

## The split this implies

Your loop body separates cleanly into `update(dt)` and `render()`, and once those are
distinct the whole rest of Part 2 gets easier — `update` mutates state, `render`
only reads it. Enforcing that read-only discipline in `render` (const, or just
discipline) prevents a large class of confusing bugs.

## Reading

- Glenn Fiedler, *Fix Your Timestep!* — the canonical article. Short, read it twice.
- Robert Nystrom, *Game Programming Patterns*, "Game Loop" chapter. Free online.
  Also read "Update Method" while you're there.

## Exercises

1. **Accumulator by hand.** Before wiring it into the game, write a console program:
   feed a hardcoded array of frame times (including one 0.8s spike) into the
   accumulator loop and print how many fixed steps each frame produces. Assert the
   totals. This is the exact test chapter 15 will reuse.
2. **Remove the cap.** Omit `MAX_STEPS`, feed in a 5-second frame, and watch the
   while loop run 300 iterations. That's the spiral of death in miniature.
3. **The split.** Separate `update(dt)` from `render()`. Make `render` take state as
   `const&`. Then try to mutate something inside `render` and read the compile error —
   that error is the invariant working.
4. **See the stutter.** Set your fixed rate to 10 Hz while rendering at 60. The
   choppiness is what interpolation fixes. Only *then* decide whether to implement it.
5. **Input timing.** Poll input once per frame, then once per fixed step. Tap a
   single-frame action in both. Describe the difference in behaviour, then pick one
   and write down why.

## Definition of done

- [ ] Simulation steps at a fixed rate with an accumulator
- [ ] Steps-per-frame is capped
- [ ] `update(dt)` and `render()` are separate functions
- [ ] Behaviour is identical on my Mac and my PC despite different refresh rates
- [ ] I wrote down where input polling lives and why
- [ ] I understand what interpolation fixes, even if I haven't added it

Next: [08 — Scenes and Lifecycle](08-scenes-and-lifecycle.md)
