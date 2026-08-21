# 09 — Input Mapping

**The problem:** `IsKeyDown(KEY_W)` is scattered through your gameplay code. So:
rebinding is impossible, gamepad support means finding every call site, and your
game logic is coupled to a specific physical key. You also can't record or replay
input, or write a test that simulates a press.

**The fix:** gameplay code asks about *actions*, never keys.

## C++ you'll meet here

- **`enum class` with an explicit underlying type** — `enum class Action : int { ... }`,
  plus a trailing `Count` member. `Count` is the idiom that lets an array size itself
  to the enum.
- **`static_cast<size_t>(action)`** — `enum class` deliberately won't convert
  implicitly, so indexing an array by it requires saying so. That friction is the
  type safety working.
- **`std::array<T, N>`** (`<array>`) — fixed-size, knows its own size, doesn't decay
  to a pointer. The right backing store for action state, not `std::map`.
- **Why not `std::unordered_map<std::string, ...>`** — hashing a string every frame,
  cache-hostile, and turns a typo into a silent runtime miss instead of a compile
  error. Worth understanding as a *rejected* option.
- **A snapshot struct** — `struct InputState { std::array<bool, Count> down; ... }`.
  Two of them (current and previous) is how `wasPressed` works.
- **`constexpr` lookup tables** — the action-to-key binding table can be built at
  compile time.
- **`std::bitset`** — a compact alternative to `array<bool>` if you want to see it.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 9, 10 and 11.

## The shape

Gameplay asks `input.isDown(Action::MoveLeft)`. A lookup table maps actions to
physical bindings. One layer of indirection; almost everything below falls out of it.

Sample your input **once per frame** into a snapshot struct, then have everything
read the snapshot. Otherwise two systems polling at different points in the frame can
disagree about whether a key was pressed, which is a genuinely horrible bug.

## Decisions this forces on you

- **`enum class` for actions.** Compile-time checked, and an array indexed by the
  enum is a fine backing store. Resist `std::map<std::string, ...>` — string keys
  are slower and turn typos into silent runtime failures instead of compile errors.
- **Held vs pressed vs released.** Chapter 03's distinction, now at the action level.
  You need at least `isDown` and `wasPressed`. Getting `wasPressed` right means
  keeping *last frame's* snapshot too — which is the main implementation wrinkle.
- **Analog input.** Gamepad sticks are floats, not bools. Does an action return
  `bool` or `float`? Unifying them is tempting and often gets messy; separate
  "button actions" and "axis actions" is usually cleaner. Look at `GetGamepadAxisMovement`.
- **Multiple bindings per action.** WASD *and* arrows *and* left stick, all mapped
  to MoveLeft. Cheap if designed in, annoying to retrofit.
- **Contexts.** "Escape" means pause in-game and back in a menu. Either the scene
  decides (simplest, do this), or you add input contexts (more machinery than you
  need yet).

## The payoff worth naming

Once input is a snapshot behind an interface, you can **feed it synthetic data.**
That means input recording, replays, demo playback, deterministic bug reproduction,
and — combined with chapter 07's fixed timestep — the ability to actually unit-test
gameplay without a window. Chapter 15 cashes that in. This is the real reason to
build the indirection, not rebinding.

## Exercises

1. **The `Count` idiom.** Define `enum class Action` ending in `Count`, then declare
   `std::array<bool, static_cast<size_t>(Action::Count)>`. Add a new action and
   confirm the array resizes with zero other edits. That's the whole trick.
2. **`wasPressed` from two snapshots.** Implement it by comparing current against
   previous. Then test the hard case: a key pressed and released within a single
   frame. Decide what should happen and whether your code does it.
3. **Purge the direct calls.** Grep for `IsKeyDown` outside your input layer. Get to
   zero. If something resists, that's a design finding worth writing down.
4. **Two bindings, one action.** Map `MoveLeft` to A, Left-arrow, and the gamepad
   stick simultaneously. Note where analog complicates a `bool` interface.
5. **Fake input.** Feed a hand-built `InputState` into your update function with no
   window open at all. If that works, you've unlocked chapter 15's testing — verify it
   by writing one assertion about movement without ever calling `InitWindow`.

## Definition of done

- [ ] No `IsKeyDown` outside the input layer
- [ ] Input is sampled once per frame into a snapshot
- [ ] Actions support both "down" and "pressed this frame"
- [ ] At least one action has two different physical bindings
- [ ] Gamepad works, or I've written down how it would plug in
- [ ] I could feed fake input in without touching gameplay code

Next: [10 — Resources and RAII](10-resources-and-raii.md)
