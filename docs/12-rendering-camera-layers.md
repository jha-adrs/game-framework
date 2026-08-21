# 12 — Rendering, Camera and Layers

**The problem:** your draw calls are interleaved with your update logic, draw order
is whatever order your code happens to run in, and the world is exactly the size of
the window. You cannot scroll, zoom, or reliably put the UI on top.

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

## Definition of done

- [ ] A camera follows something, smoothly and frame-rate independently
- [ ] Camera is clamped to level bounds
- [ ] UI stays fixed while the world scrolls
- [ ] I can convert a mouse click into a world position
- [ ] Draw order is deliberate, not accidental
- [ ] I know what `RenderTexture2D` is for, even if unused

Next: [13 — Audio, UI and Debug Tools](13-audio-ui-debug.md)
