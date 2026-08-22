# 05 — Collision and Your First Real Game

Goal: **finish** something. Small, complete, has a win state and a lose state.

This is the most important chapter in Part 1. Not because collision is hard, but
because finishing is. Most self-taught game programmers have thirty half-built
prototypes and zero games, and the skills you're missing are all in the last 10%.

## C++ you'll meet here

- **`enum class` + `switch`** — for `Title` / `Playing` / `GameOver`. Omit a case and
  `-Wswitch` warns you, which is exactly the safety net you want as states multiply.
- **`std::vector<T>`** — your brick array. `push_back`, `size()`, range-for, and the
  removal problem below.
- **Range-for with `&`** — `for (auto& brick : bricks)` mutates; without the `&` you
  modify copies and nothing happens. The single most common silent no-op in C++.
- **Removing while iterating** — erasing inside a range-for is undefined behaviour.
  Learn `std::erase_if` (C++20) or the erase-remove idiom, or use swap-and-pop.
  Chapter 11 goes deeper; you need *one* working approach now.
- **`struct` composition** — a `Ball` that *has* a `Vector2 pos` and `Vector2 vel`,
  rather than four loose floats. Grouping related data is the cheapest design win here.
- **`const` member functions / `const&` params** — `bool isColliding(const Rect& a,
  const Rect& b)` documents that collision *detection* changes nothing, which keeps
  detection and resolution mentally separate.
- **Functions returning `bool`** — and naming them as questions (`isDead`, `hasWon`).

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 9 and 10.

## Collision: two shapes, that's all you need

raylib gives you these — find them in `raylib.h`:

- `CheckCollisionRecs(Rectangle, Rectangle)`
- `CheckCollisionCircles(Vector2, float, Vector2, float)`
- `CheckCollisionCircleRec(Vector2, float, Rectangle)`
- `CheckCollisionPointRec`, `CheckCollisionPointCircle` — mouse hit-testing
- `GetCollisionRec` — the *overlap* rectangle of two rects. Underrated; it tells you
  how deep the overlap is, which is what you need to push things apart.

**Write AABB overlap yourself before using `CheckCollisionRecs`.** It's four
comparisons and it's the single most useful thing in 2D games. The trick is that
it's easier to test for *separation* — if any one of "A is entirely left of B",
"entirely right", "entirely above", "entirely below" is true, they don't touch.
Negate that. Deriving it this way is much clearer than memorising the four
inequalities.

Also convince yourself why AABB is called *axis-aligned*: rotate a box and this
math stops working. Rotated collision (SAT) is a much bigger topic and you do not
need it.

## Detection vs resolution — the part tutorials skip

`CheckCollisionRecs` returns a `bool`. It tells you *that* you overlap. It says
nothing about what to do, and "what to do" is the actual work.

Consider: your player moves down-right into a wall corner. You detect overlap. Do
you stop horizontal movement, vertical, both? Push out along the smallest axis of
overlap? If you naively zero both, the player sticks to walls and can't slide along
them — which feels awful and is instantly recognisable in bad platformers.

Approaches worth knowing about:

- **Move-and-resolve per axis.** Move x, check, push out of x. Then move y, check,
  push out of y. Two separate passes. Simple, robust, gives free wall-sliding, and
  is what a great many 2D platformers actually ship.
- **Push out along minimum overlap.** Use `GetCollisionRec`, find the shallower
  axis, push that way. One pass, but corner cases are genuinely fiddly.
- **Swept collision.** Compute *when* during the frame the hit happens, move exactly
  that far. Correct, and the real fix for tunnelling. Overkill right now.

Start with per-axis. Understanding why it produces sliding is worth more than a
clever one-liner you can't debug.

## Pick one game and finish it

Pick **one**. Resist scope. Each is genuinely finishable in a few sessions.

- **Pong** — easiest. Two paddles, a ball, angle-off-paddle, score to 5. The best
  choice if you want to be done fast, and "reflect the ball based on where it hit
  the paddle" is a real feel decision.
- **Breakout** — a grid of bricks, so you get your first "array of things" and your
  first "remove from a collection while iterating" problem. Best learning per hour.
- **Asteroids** — rotation, thrust, screen wrap, spawning. Most fun, most scope.
  Choose it only if you accept it'll take longer.

I'd pick **Breakout**. The brick array is exactly the pressure that makes chapter 11
make sense later.

## The last 10% — this is the actual curriculum

Everything above is the easy part. These are the things you have never done:

- **Game states.** Title → playing → game over → back to title. Right now your
  program has exactly one state, implicitly. Adding a second forces the question
  chapter 08 answers. Do it the crude way first — an `enum` and a `switch` — and
  feel where it strains.
- **Restart without relaunching.** Requires knowing exactly what "the game state"
  *is*, so you can reset it. Most people discover their state is scattered across
  fifteen variables and some of it lives in the wrong place. This is a genuinely
  clarifying exercise.
- **Score, and displaying it.** `DrawText` + `TextFormat`.
- **A lose condition** that actually triggers and is reachable.
- **Feel.** The gap between "technically a game" and "fun" is almost entirely
  feedback: a flash on hit, a tiny screen shake, a particle, a sound. Try adding
  *one* and notice how much it changes. This is a real discipline (people call it
  "game feel") and it's mostly cheap tricks.
- **Give it to someone.** A friend, a sibling, anyone. Watch them play without
  explaining. You will learn more in ninety seconds than in a week of solo work.

## Exercises

1. **Derive AABB.** Write overlap from the four *separation* cases and negate, rather
   than copying four inequalities. Then test it against `CheckCollisionRecs` on a few
   hundred random pairs and assert they agree. Any disagreement is a real edge case
   worth understanding — start with exactly-touching edges.
2. **The `auto` no-op, for real.** Loop your bricks with `for (auto b : bricks)` and
   set `b.alive = false` on collision. Nothing dies. Fix it with one character.
3. **Removal, three ways.** Implement brick removal with (a) swap-and-pop,
   (b) mark-dead-then-sweep, (c) `std::erase_if`. Keep one. Write down why.
4. **Detection vs resolution.** Make your player move diagonally into a wall corner.
   First zero both axes on collision — feel it stick. Then resolve per axis and feel
   it slide. That difference is the whole point of the chapter.
5. **State machine.** `enum class GameState` plus `switch`. Deliberately omit
   `GameOver` from the switch and read the `-Wswitch` warning. Then handle it.
6. **Reset.** Write a function that returns the game to its starting state. If you
   can't do it in one obvious place, your state is scattered — that's the finding,
   and fixing it is the exercise.

## Definition of done

- [ ] I wrote AABB overlap myself and can derive it from the separation cases
- [ ] Collision *resolution* works — things don't overlap or stick
- [ ] There is a title screen, a play state, and a game over screen
- [ ] I can restart without relaunching the executable
- [ ] There is a score on screen
- [ ] It is winnable and losable
- [ ] It has at least one piece of juice (flash, shake, or sound)
- [ ] It runs on macOS and Windows from the same commit
- [ ] **A real person other than me has played it**
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

Stop here for a while. Play it. Notice what annoyed you to write.
That annoyance is the input to Part 2.

Next: [06 — When to Abstract](06-when-to-abstract.md)
