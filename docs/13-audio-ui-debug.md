# 13 — Audio, UI and Debug Tools

Three small subsystems. Grouped because none needs a chapter of its own, and because
the third is the one that pays for itself fastest.

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

## Definition of done

- [ ] Sound effects play once per event, not per frame
- [ ] Repeated sounds have pitch variation
- [ ] Music streams and doesn't cut out
- [ ] Clickable buttons, and I wrote the first one myself
- [ ] Text is crisp and I can centre it
- [ ] **F1 debug overlay**
- [ ] **Collision shapes can be visualised**
- [ ] **I can pause and single-step the simulation**

Next: [14 — Cross-Platform Reality](14-cross-platform-windows.md)
