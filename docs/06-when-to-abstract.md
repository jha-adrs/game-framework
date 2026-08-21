# 06 — When to Abstract

**The gate into Part 2.** Don't skip it. This chapter is the difference between
building an engine and building a monument to an engine you never used.

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

## Definition of done

- [ ] I read my own chapter-05 code start to finish
- [ ] I wrote down actual answers to the four questions
- [ ] I have a list of Part 2 chapters ordered by my own pain, not the numbering
- [ ] I've considered writing a second small game before abstracting anything

Next: [07 — Loop Architecture](07-loop-architecture.md)
