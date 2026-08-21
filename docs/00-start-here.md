# 00 — Start Here

## What this is

A staged path from "I have a compiler" to "I have an engine I understand."
It is deliberately *not* an engine design document. Engines are what you get
when you notice the same code three times and extract it — not what you draw
on a whiteboard before writing a game.

## How to use it

- Do the chapters **in order**. Each one assumes the previous one runs.
- Don't read ahead and start building chapter 11 during chapter 3. That is the
  single most common way self-taught engine projects die.
- When a chapter says "look this up in `raylib.h`", look it up in `raylib.h`.
  Do not skip to a tutorial. The header is the documentation and learning to
  read a C header is half the skill you're here for.

## The three parts

### Part 1 — Foundations (chapters 01–05)
Detailed and prescriptive. You have no intuition yet, so these hold your hand
on concepts and hand you build plumbing outright. Ends with a small, complete,
finished game.

### Part 2 — Toward a framework (chapters 06–13)
Thinner on purpose. Each chapter names a problem, explains why it exists, and
lists the decisions you'll have to make — but does not make them for you.
By the time you get here you'll have opinions, and your opinions will be better
informed than any decision made in advance.

### Part 3 — Engineering (chapters 14–16)
Checklists and pointers. Cross-platform reality, testing, profiling, shipping.

## Progress

Tick these off as you go. Honest ticks only — "it compiles" is not "it works."

**Part 1 — Foundations**
- [ ] 01 — Toolchain builds an empty raylib program on macOS
- [ ] 01 — ...and on Windows
- [ ] 02 — Window opens, background color cycles over time
- [ ] 03 — A shape I can move with the keyboard
- [ ] 04 — Movement speed is identical at 30, 60 and 144 FPS
- [ ] 05 — One complete tiny game with a win/lose state

**Part 2 — Toward a framework**
- [ ] 06 — Listed what actually repeated across my code
- [ ] 07 — Fixed-timestep update, decoupled render
- [ ] 08 — Scene switching (menu → game → game over)
- [ ] 09 — Input mapped to actions, not keys
- [ ] 10 — Assets loaded and freed without leaks
- [ ] 11 — Entities in contiguous storage
- [ ] 12 — Camera and draw ordering
- [ ] 13 — Audio + an F1 debug overlay

**Part 3 — Engineering**
- [ ] 14 — Same commit builds and runs on both machines
- [ ] 15 — Tests on the non-graphics logic; profiled one real bottleneck
- [ ] 16 — A packaged build a friend can double-click

## A note on getting stuck

Getting stuck is the job. But there is a difference between *productive* stuck
(you don't understand the concept) and *unproductive* stuck (a build flag is
wrong). Chapter 01 has an error-triage table specifically so build-plumbing
problems cost you minutes instead of evenings.

Next: [01 — Toolchain and Build](01-toolchain-and-build.md)
