# 07 — Loop Architecture

**The problem:** chapter 04 left you with variable-timestep physics. It's
non-deterministic (the same inputs produce different results on different machines),
it tunnels through walls on a hitch, and it makes networking or replays impossible.
Your Mac and your 144 Hz PC will not agree.

**The fix:** decouple simulation rate from render rate.

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

## Definition of done

- [ ] Simulation steps at a fixed rate with an accumulator
- [ ] Steps-per-frame is capped
- [ ] `update(dt)` and `render()` are separate functions
- [ ] Behaviour is identical on my Mac and my PC despite different refresh rates
- [ ] I wrote down where input polling lives and why
- [ ] I understand what interpolation fixes, even if I haven't added it

Next: [08 — Scenes and Lifecycle](08-scenes-and-lifecycle.md)
