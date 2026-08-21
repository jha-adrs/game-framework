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

Every chapter from 02 on has two extra sections:

- **C++ you'll meet here** — near the top. The language features this chapter
  introduces and why *this* problem is what makes them worth learning. Read it
  before you start writing. It links back to
  [01a — C++ Refresher](01a-cpp-refresher.md) when something needs more than a
  reminder.
- **Exercises** — just before the checklist. Small, and several deliberately
  make something break so you can see the failure rather than read about it.
  The ones that say "run this under ASan" are the highest-value ones on the
  roadmap; don't skip them because the code "looks fine."

The C++ is spread across the chapters on purpose. Learning move semantics in the
abstract is miserable; learning it because a copied `Texture2D` just double-freed
a GPU handle takes about ten minutes and sticks permanently.

## The three parts

### Interlude — 01a, C++ Refresher
Not a chapter. The C++-shaped subset you need for everything that follows, for
someone fluent in another language who has forgotten the details. Its exercises
are plain console programs — no raylib, no CMake, instant feedback. Read it after
chapter 01 and return to it whenever a **C++ you'll meet here** section names
something fuzzy.

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
- [ ] 01a — All eight C++ refresher exercises run and explained
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

A third kind is worth naming: **stuck because C++ did something you didn't expect**
— a change that didn't stick, a value that was garbage, a crash with a useless
stack trace. That is almost always one of value semantics, a dangling
reference/iterator, or uninitialised memory. All three are in the refresher, and
all three are things sanitizers find in seconds. Reach for
`-fsanitize=address,undefined` earlier than feels necessary.

Next: [01 — Toolchain and Build](01-toolchain-and-build.md)
