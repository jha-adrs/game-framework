# 12 — Rendering, Camera and Layers

**The problem:** your draw calls are interleaved with your update logic, draw order
is whatever order your code happens to run in, and the world is exactly the size of
the window. You cannot scroll, zoom, or reliably put the UI on top.

## C++ you'll meet here

- **Lambdas** — `[](const Sprite& a, const Sprite& b) { return a.layer < b.layer; }`.
  This is where they finally have an obvious use. Understand the capture list:
  `[]` captures nothing, `[&]` by reference, `[=]` by copy, `[this]` the enclosing
  object. Capturing by reference and outliving the referent is a real bug.
- **`std::sort`** (`<algorithm>`) — and the requirement that your comparator be a
  *strict weak ordering*. A comparator using `<=` instead of `<` is undefined
  behaviour and really does crash in release builds. Non-obvious and worth knowing.
- **`std::stable_sort`** — preserves relative order of equal elements. For draw
  ordering this is often what you actually want, since it keeps insertion order within
  a layer.
- **`std::function`** (`<functional>`) — type-erased callable. Convenient, but it can
  heap-allocate and it prevents inlining. Fine for a handful of draw commands, wrong
  for a per-entity per-frame hot path. Knowing *when it costs* is the lesson.
- **Sorting by multiple keys** — `std::tie(layer, y)` (`<tuple>`) gives you
  lexicographic comparison in one line.
- **A draw-command struct** — separating "decide to draw" from "draw" means a
  `std::vector<DrawCmd>` you sort then flush. Plain data, easy to reason about.

Fuzzy? [01a — C++ Refresher](01a-cpp-refresher.md) sections 6 and 9.

## Camera: world space vs screen space

`Camera2D` is a struct with `target` (the world point to centre on), `offset` (where
that point lands on screen), `rotation` and `zoom`. Wrap drawing in
`BeginMode2D(camera)` / `EndMode2D()` and raylib transforms world coordinates into
screen coordinates for you.

**The conceptual split is the whole chapter:** there are now two coordinate systems.
World space is where your entities live. Screen space is pixels. The camera is the
transform between them, and confusing the two is the source of essentially every
camera bug.

Consequences to work through yourself:

- **UI must be drawn outside `BeginMode2D`**, or your health bar scrolls away with
  the world.
- **Mouse position is in screen space.** Clicking on a world object requires
  `GetScreenToWorld2D`. Its inverse `GetWorldToScreen2D` is how you put a
  world-anchored label on screen. You will need both.
- **`offset` is usually half the screen size** to centre the target. Work out why,
  and what happens on window resize.
- **Camera follow needs smoothing.** Snapping the camera to the player exactly is
  jarring. Lerp toward the target — and note the lerp factor must account for delta
  time or it's frame-rate dependent (chapter 04, again, from a new angle).
- **Clamp the camera to level bounds** so you don't show the void past the edges.

## Draw order and layers

Immediate mode means **draw order is just call order**: last drawn is on top. That
works until "the player should be behind that tree but in front of the floor" spans
multiple update systems.

Options, cheapest first:

- **Explicit ordered passes.** `drawBackground(); drawEntities(); drawUI();` —
  literally just calling things in the right order. Honestly fine for a long time,
  and self-documenting.
- **A layer enum per entity**, then draw layer by layer. Small step up, handles most
  2D games.
- **A collected draw list**, sorted by (layer, y) before flushing. Needed for
  top-down games where sorting by y-position gives correct depth. This is where a
  real "renderer" starts to exist as a thing — and note it decouples *deciding to
  draw* from *drawing*, which is why it's more flexible.

## Things worth knowing exist

- **`RenderTexture2D`** — draw into an off-screen buffer, then draw that buffer to
  the screen. This is how you do pixel-perfect low-res games (render at 320×180,
  upscale with `NEAREST` filtering), full-screen shader effects, and transitions.
  Note the classic gotcha: render textures are **y-flipped**, so you draw them with
  a negative source height.
- **Texture atlases / sprite sheets.** `DrawTextureRec` draws a sub-rectangle. One
  big texture with many sprites beats many small textures — fewer GPU state changes.
- **`DrawTexturePro`** — source rect, dest rect, origin, rotation. The one draw call
  that does everything; learn its parameters properly, especially that `origin` is
  the rotation pivot *and* shifts the destination.
- **Shaders.** `LoadShader`, GLSL, uniforms. Genuinely fun, a real rabbit hole, and
  cross-platform GLSL version differences are a real annoyance. Deliberately out of
  scope here — but this is the most rewarding optional detour on the list.
- **Resolution independence.** Design for a virtual resolution and scale to the
  window, or your game breaks on your PC's monitor. Handle `IsWindowResized`.

## Exercises

1. **Capture semantics.** Write a lambda capturing a local by reference, return it
   from the function, then call it. Run under ASan. That's a dangling capture, and it
   looks completely innocent.
2. **Break `std::sort`.** Write a comparator using `<=` instead of `<`, sort a few
   thousand elements in a Release build. It may crash, it may silently corrupt. Then
   read why "strict weak ordering" is a requirement rather than a suggestion.
3. **Sort by two keys.** Order sprites by layer then y, first with a hand-written
   nested comparison, then with `std::tie`. Compare readability.
4. **Stable vs not.** Give several sprites the same layer, sort with both `sort` and
   `stable_sort`, and observe the flicker difference across frames when order isn't
   preserved. This is a real bug that looks like a rendering glitch.
5. **`std::function` cost.** Store 10,000 draw operations as `std::function` and as a
   plain struct with a switch. Time both in Release. Now you have a number instead of
   an opinion.
6. **World vs screen.** Click to spawn something at the mouse position while the
   camera is scrolled and zoomed. Get it right with `GetScreenToWorld2D`. Then get it
   deliberately wrong to see what the bug looks like.

## Definition of done

- [ ] A camera follows something, smoothly and frame-rate independently
- [ ] Camera is clamped to level bounds
- [ ] UI stays fixed while the world scrolls
- [ ] I can convert a mouse click into a world position
- [ ] Draw order is deliberate, not accidental
- [ ] I know what `RenderTexture2D` is for, even if unused

Next: [13 — Audio, UI and Debug Tools](13-audio-ui-debug.md)
