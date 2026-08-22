# 13 — Audio, UI and Debug Tools

Three small subsystems. Grouped because none needs a chapter of its own, and because
the third is the one that pays for itself fastest.

## C++ you'll meet here

- **`std::string` and `.c_str()`** — raylib takes `const char*`, so this is your
  bridge. Note the lifetime hazard: `.c_str()` on a temporary dangles as soon as the
  full expression ends.
- **C variadic functions** — raylib's `TextFormat` is `printf`-style varargs. They are
  **not type-checked**: passing an `int` where `%s` is expected is undefined behaviour,
  not a compile error. Also, `TextFormat` returns a pointer into a rotating internal
  static buffer, so holding onto the result across frames is a bug. Both worth knowing.
- **`std::format`** (C++20) / **fmt** — the type-safe replacement. Worth knowing what
  you're missing on C++17, and a reason you might bump the standard.
- **Lambdas vs function pointers** — for a button's on-click. A capture-less lambda
  converts to a function pointer; a capturing one does not. That surprises people.
- **`std::string_view`** (C++17) — a non-owning view over characters. The right
  parameter type for "some text I only read," avoiding a copy. Same dangling caveat
  as `.c_str()`.
- **Static local variables** — `static bool showDebug = false;` inside a function
  persists across calls. Genuinely handy for debug toggles, and a good place to
  understand that `static` means at least three different things in C++ depending on
  context.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 8 and 9.

## Audio

`InitAudioDevice()` before loading anything, `CloseAudioDevice()` at the end — the
same context-lifetime constraint as textures in chapter 10, so the same RAII rules
apply.

- **`Sound`** — short, fully loaded in memory. Effects.
- **`Music`** — streamed from disk. Needs `UpdateMusicStream()` **every frame**, and
  forgetting that is the classic "music plays for half a second and stops."
- `SetSoundVolume`, `SetMasterVolume`, `PlaySound`, `PauseMusicStream`.

Things you'll actually hit:

- **The same sound triggering 60 times a second** because you played it on
  `IsKeyDown` instead of `IsKeyPressed`, or once per collision frame instead of once
  per collision *event*. It sounds like distortion. Very common.
- **Pitch variation.** `SetSoundPitch` with a small random offset per play stops
  repeated sounds from feeling mechanical. Two lines, disproportionate improvement —
  this is the cheapest "game feel" win available anywhere.
- **A master volume you can actually mute.** You'll be testing for hours.
- Formats: `.wav` for effects (small, no decode cost), `.ogg` for music.

## UI

You need buttons and text. You do not need a UI framework.

- **Roll your own immediate-mode buttons first.** A function taking a rect and a
  label, returning `bool` for "clicked this frame", using `CheckCollisionPointRec`
  and the mouse functions. It's about 15 lines and it will teach you what
  immediate-mode UI *is* — the same "no retained objects" idea as chapter 02, applied
  to widgets.
- **raygui** is raylib's official companion UI library — single header, drop it in.
  Worth using for debug panels and settings screens. Not worth using for your game's
  actual styled menus.
- **Fonts.** The default font is bitmap and looks rough scaled up. `LoadFontEx` with
  a size and a TTF gives you crisp text. Text *measurement* (`MeasureTextEx`) is what
  you need for centring and wrapping, and you'll want it immediately.

## Debug tools — build these, they pay for themselves

This is the highest return-on-effort item in Part 2. Every hour here saves several
later, and it's the habit that separates people who make progress from people who
guess.

- **An F1 overlay.** FPS, frame time, entity counts, player position, current scene.
  `DrawFPS` exists but roll your own so you can add whatever you need.
- **Collision shape visualisation.** Toggle to draw every hitbox as a wireframe.
  The single most useful debug feature in any 2D game — the number of "why didn't
  that hit" bugs this resolves instantly is remarkable.
- **A pause-and-step key.** Freeze the simulation, advance exactly one fixed step per
  keypress. Chapter 07's fixed timestep is what makes this possible, and it turns
  "it happens too fast to see" into a solved problem forever.
- **Live-tweakable values.** Adjust jump height or speed at runtime instead of
  editing, recompiling, relaunching, and replaying to the test spot. raygui sliders
  are enough. This changes tuning from a chore into something you'll actually do,
  which is how games get good.
- **Slow-motion / fast-forward.** Scale your fixed timestep. Trivial once 07 exists.

## Exercises

1. **Varargs are not type-safe.** Call `TextFormat("%d", "hello")` and
   `TextFormat("%s", 42)`. Note they compile. Observe what happens at runtime. Then
   turn on `-Wformat` and see the compiler catch it *for the printf-family* — and
   consider why it can't help you in general.
2. **The rotating buffer.** Store the `const char*` from `TextFormat` in a variable,
   call `TextFormat` a dozen more times, then draw the stored pointer. Watch it change
   out from under you.
3. **Sound spam.** Play a hit sound on `IsKeyDown` instead of `IsKeyPressed`. Listen
   to the distortion. Then fix it. Then add random pitch variation and A/B the two —
   this is the cheapest game-feel win on the whole roadmap.
4. **Your own button.** Write an immediate-mode button: takes a rect and a label,
   returns `bool` for clicked-this-frame. About 15 lines. Then explain how it relates
   to chapter 02's immediate-mode idea.
5. **Capture-less conversion.** Assign a capture-less lambda to a function pointer.
   Then add a capture and watch it fail to compile. Understand why.
6. **The debug overlay.** F1 toggle with a `static` local. FPS, worst frame time,
   entity count, current scene. Then add collision-shape wireframes and a
   pause-and-single-step key. These three are the highest-value tools on the roadmap;
   build them properly.

## Definition of done

- [ ] Sound effects play once per event, not per frame
- [ ] Repeated sounds have pitch variation
- [ ] Music streams and doesn't cut out
- [ ] Clickable buttons, and I wrote the first one myself
- [ ] Text is crisp and I can centre it
- [ ] **F1 debug overlay**
- [ ] **Collision shapes can be visualised**
- [ ] **I can pause and single-step the simulation**
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

Next: [14 — Cross-Platform Reality](14-cross-platform-windows.md)
