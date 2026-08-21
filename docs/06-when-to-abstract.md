# 06 — When to Abstract

**The gate into Part 2.** Don't skip it. This chapter is the difference between
building an engine and building a monument to an engine you never used.

## C++ you'll meet here

This chapter is where you split `main.cpp`, so it's where C++'s compilation model
stops being trivia and starts mattering.

- **Translation units** — each `.cpp` compiles independently into one object file.
  They know nothing about each other except through headers.
- **Header / source split** — declarations in `.h`, definitions in `.cpp`. This is
  the same compile-vs-link distinction from chapter 01, now organising your files.
- **`#pragma once`** — top of every header. Without it, a header included twice in
  one translation unit redefines everything.
- **The One Definition Rule (ODR)** — a non-`inline` function defined in a header and
  included from two `.cpp` files is a duplicate-symbol *link* error. Exercise 6 in the
  refresher was rehearsal; now it's real.
- **`inline`** — permits a definition in a header. Necessary for small helpers you
  want visible everywhere.
- **Forward declaration** — `struct Player;` lets a header reference a type without
  including its definition. This is how you stop one header change from recompiling
  everything, and how you break circular includes.
- **Anonymous namespace / `static`** — file-local helpers that don't leak into other
  translation units and can't collide.
- **CMake: adding source files** — every new `.cpp` goes in `add_executable`, or the
  linker won't see its definitions. Forgetting is a very common self-inflicted
  undefined-symbol error.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 7 and 12.

## Read your own code first

Open your finished chapter-05 game. Actually read it. Then write down, in a scratch
file, honest answers to:

1. **What did I copy-paste?** Literally — where is the same shape of code twice or
   more? Those are your real abstraction candidates. Not the ones a tutorial told
   you about.
2. **What was annoying to change?** If tweaking the ball's speed meant editing three
   places, that's a design problem your code is reporting to you.
3. **What did I get wrong more than once?** Repeated bugs point at a missing
   invariant that a type or function could enforce.
4. **What's in `main.cpp` that clearly isn't "main"?** Length alone isn't a problem.
   Mixed responsibilities are.

Everything in Part 2 should be justified by an answer you wrote here. If a chapter
solves a problem you don't have, **skip it and come back when you do.**

## The rule of three

Extract on the **third** occurrence, not the first.

One occurrence is a fact. Two might be coincidence. Three is a pattern, and by then
you've seen enough variation to know which parts genuinely differ — which is
precisely the information you need to draw the interface correctly.

Abstracting at one is guessing. And a wrong abstraction is much more expensive than
duplication, because duplication is easy to delete and a bad interface has to be
un-picked from every caller.

## What "framework" should mean here

Not Unity. Not an ECS you read a blog post about. For you, right now:

> The code that would be the same in your *next* game.

That's it. A framework is the residue of two games, not the plan for one. Which
means: **the highest-value thing you can do after chapter 05 is write a second,
different small game.** The overlap between them is your framework, discovered
rather than invented. That is genuinely the fastest route, even though it feels like
a detour.

## Order Part 2 yourself

The chapter numbers are a suggestion, not a dependency graph. Only two real
constraints:

- **07 (loop architecture) is worth doing early** — it touches everything, so doing
  it later means redoing more.
- **10 (resources/RAII) should come before you load many assets**, or you'll be
  chasing leaks.

Everything else: do it when it hurts. Sort the remaining chapters by "how much did
this actually annoy me in chapter 05" and work top-down.

## Exercises

1. **The audit.** The four questions above, answered in writing, committed. This is
   the actual exercise; everything else here is mechanical.
2. **First split.** Move your collision functions into `collision.h` / `collision.cpp`.
   Add the `.cpp` to `add_executable`. Then *deliberately forget* to add it and read
   the resulting link error so you recognise it next time.
3. **Trigger an ODR error.** Put a non-inline function definition in a header,
   include it from two `.cpp` files, link. Read the error. Fix it with `inline`, then
   instead by moving the definition. Understand both.
4. **Forward declaration.** Find a header that includes another only to name a type in
   a signature. Replace the include with a forward declaration. Confirm it still
   builds, and articulate what you gained.
5. **Count the duplication.** Grep your own code for the shape you copy-pasted most.
   If it appears twice, leave it. Three times, extract it. Being disciplined about
   this exact judgement is the skill the chapter is teaching.

## Definition of done

- [ ] I read my own chapter-05 code start to finish
- [ ] I wrote down actual answers to the four questions
- [ ] I have a list of Part 2 chapters ordered by my own pain, not the numbering
- [ ] I've considered writing a second small game before abstracting anything

Next: [07 — Loop Architecture](07-loop-architecture.md)
