# 09 — Input Mapping

**The problem:** `IsKeyDown(KEY_W)` is scattered through your gameplay code. So:
rebinding is impossible, gamepad support means finding every call site, and your
game logic is coupled to a specific physical key. You also can't record or replay
input, or write a test that simulates a press.

**The fix:** gameplay code asks about *actions*, never keys.

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

## Definition of done

- [ ] No `IsKeyDown` outside the input layer
- [ ] Input is sampled once per frame into a snapshot
- [ ] Actions support both "down" and "pressed this frame"
- [ ] At least one action has two different physical bindings
- [ ] Gamepad works, or I've written down how it would plug in
- [ ] I could feed fake input in without touching gameplay code

Next: [10 — Resources and RAII](10-resources-and-raii.md)
