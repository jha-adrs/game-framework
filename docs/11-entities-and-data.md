# 11 — Entities and Data Layout

**The problem:** you have bricks, bullets, enemies and particles. Right now they're
probably separate `vector`s with near-identical update loops, or one `vector` of a
fat struct with unused fields. Spawning and despawning mid-iteration causes bugs.

**The warning:** this is the chapter where people rewrite their game as an ECS
because they read an article, and then never ship. Read the escalation ladder below
and stop at the lowest rung that solves your actual problem.

## C++ you'll meet here

- **`std::vector` internals** — `size()` vs `capacity()`, geometric growth, and
  `reserve()`. Knowing the difference is what makes the invalidation rules make sense
  rather than seem arbitrary.
- **Iterator / pointer / reference invalidation** — `push_back` past capacity
  invalidates *everything* pointing into the vector. `erase` invalidates from the
  erased position onward. Short list, worth memorising exactly.
- **`std::erase_if`** (C++20) / **erase-remove idiom** (`std::remove_if` + `erase`,
  C++17). Know which standard you're on — chapter 01 set `CMAKE_CXX_STANDARD 17`, so
  `erase_if` is unavailable unless you bump it. That's a real decision to make here.
- **Swap-and-pop** — O(1) removal that reorders. Fine when order doesn't matter, and
  it usually doesn't.
- **`size_t` vs `int`** — `.size()` is unsigned. Mixing signs in a loop condition is
  `-Wsign-compare`, and reverse loops with `size_t` underflow catastrophically at zero.
- **`std::optional<T>`** (`<optional>`, C++17) — "maybe an entity," without a
  sentinel index or a null pointer.
- **Generational handles** — `struct Handle { uint32_t index; uint32_t generation; };`
  Plain data, no language feature needed, kills a whole bug class.
- **Templates, first real contact** — if you write a `SlotMap<T>`, you're writing a
  template. Keep it in one header, keep it small.
- **`std::span`** (C++20) — a non-owning view over contiguous data. Note it exists;
  it's the clean way to pass "a range of entities" once you're on C++20.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) section 9, and exercise 5 there.

## The escalation ladder

**Rung 1 — `std::vector<Thing>`.** One vector per kind. Contiguous, cache-friendly,
trivially debuggable. **This handles thousands of entities without breaking a sweat**
and is almost certainly where you should stay for a long time.

**Rung 2 — vector + generational handles.** The moment something needs to *refer* to
another entity ("this bullet was fired by that player"), you need a stable
reference. A raw pointer or index breaks when the vector reallocates or the entity
dies and its slot is reused — you get the classic bug where a bullet damages a newly
spawned enemy that happens to occupy the dead one's index. The fix: a handle of
`{index, generation}`, with a generation counter bumped on every reuse. Cheap, and it
kills a whole bug class permanently. **This rung is genuinely worth learning.**

**Rung 3 — struct-of-arrays.** Instead of `vector<Entity>` with position, velocity
and sprite together, keep `vector<Vector2> positions; vector<Vector2> velocities;`.
A loop that touches only positions then reads only the data it needs, so every cache
line is fully used. Real wins — but only at high entity counts and only if you have
*measured* that you're memory-bound.

**Rung 4 — a real ECS.** Composition over inheritance, archetypes, systems querying
component sets. Powerful and genuinely the right answer for large games. **You almost
certainly do not need it**, and you should not write one until you can point at a
specific problem in your own code that rungs 1–3 cannot solve. If you get there, use
[EnTT](https://github.com/skypjack/entt) rather than writing your own.

Be honest about which rung your actual pain is on.

## Problems you'll hit regardless of rung

- **Removing while iterating.** Erasing from a vector inside a range-for over it is
  undefined behaviour. Standard fixes: swap-and-pop (fast, reorders — fine if order
  doesn't matter), or mark-dead-then-sweep-at-end-of-frame (preserves order,
  predictable). Look up `std::erase_if` / the erase-remove idiom. Pick one and be
  consistent; mixing them is how you get "sometimes an enemy survives."
- **Spawning while iterating.** `push_back` can reallocate and invalidate the
  iterator you're standing on. Queue spawns, apply after the loop.
- **Iterator invalidation in general.** Learn precisely when `vector` invalidates
  iterators, pointers and references. This is a top source of C++ crashes and the
  rules are short enough to memorise.
- **Reserve up front.** If you know the rough max, `reserve()` avoids reallocation
  churn entirely. One line, free win.

## The inheritance temptation

`class Enemy : public Entity` looks natural and coming from Java it's the obvious
move. It goes wrong specifically when something needs to be two things at once — a
destructible *and* a pickup *and* an enemy — and you find yourself either duplicating
code across the hierarchy or growing a base class that knows about everything.

You don't have to pre-empt this. Just recognise the smell when it arrives, because
recognising it is what makes composition (and eventually ECS) feel like an answer
rather than a fashion.

## Reading

- *Game Programming Patterns* — "Component", "Object Pool", "Data Locality".
  The data locality chapter is the clearest explanation of why rung 3 exists.

## Exercises

1. **Watch reallocation.** Print `size()` and `capacity()` after each of 20
   `push_back`s. Note the growth pattern. Then `reserve(20)` up front and print again.
2. **Invalidation, caught.** Hold a pointer to `v[0]`, push past capacity, read
   through the pointer. Run under ASan. Read the report carefully — it names both the
   bad access and the old allocation.
3. **The reuse bug.** Store a bare index as an entity reference. Kill that entity,
   spawn a new one into the same slot, then use the stale index. Watch the bullet
   damage the wrong enemy. Then implement `{index, generation}` and watch it become
   impossible.
4. **Removal, measured.** Implement swap-and-pop and mark-and-sweep over 10,000
   entities. Time both in a Release build. Then check whether the difference matters
   at *your* actual entity count — the honest answer is usually no, and knowing that
   is the point.
5. **`size_t` underflow.** Write a reverse loop with `for (size_t i = v.size() - 1; i >= 0; --i)`
   over an empty vector. Predict what happens, then run it. Fix it two ways.
6. **Struct-of-arrays, benchmarked.** Convert one hot loop to parallel arrays.
   Measure. If it's not faster at your entity count, revert it and write down what
   you learned. A reverted optimisation you *measured* is a success.

## Definition of done

- [ ] Entities live in contiguous storage
- [ ] I can spawn and despawn mid-frame without bugs
- [ ] One entity can safely refer to another (handles, if needed)
- [ ] I can state which rung I'm on and what would push me up one
- [ ] I did **not** write an ECS unless I can name the problem it solved
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

Next: [12 — Rendering, Camera, Layers](12-rendering-camera-layers.md)
