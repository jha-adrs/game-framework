# 11 — Entities and Data Layout

**The problem:** you have bricks, bullets, enemies and particles. Right now they're
probably separate `vector`s with near-identical update loops, or one `vector` of a
fat struct with unused fields. Spawning and despawning mid-iteration causes bugs.

**The warning:** this is the chapter where people rewrite their game as an ECS
because they read an article, and then never ship. Read the escalation ladder below
and stop at the lowest rung that solves your actual problem.

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

## Definition of done

- [ ] Entities live in contiguous storage
- [ ] I can spawn and despawn mid-frame without bugs
- [ ] One entity can safely refer to another (handles, if needed)
- [ ] I can state which rung I'm on and what would push me up one
- [ ] I did **not** write an ECS unless I can name the problem it solved

Next: [12 — Rendering, Camera, Layers](12-rendering-camera-layers.md)
