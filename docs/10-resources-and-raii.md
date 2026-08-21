# 10 — Resources and RAII

**This is the chapter where you actually learn C++.** Everything before this you
could have done in any language. This is the part that has no equivalent in
JS or Java, and raylib's C API is an unusually good teacher because it makes the
problem impossible to ignore.

## The problem

raylib resources come in `Load` / `Unload` pairs:

| Load | Unload |
|---|---|
| `LoadTexture` | `UnloadTexture` |
| `LoadSound` | `UnloadSound` |
| `LoadFont` | `UnloadFont` |
| `LoadMusicStream` | `UnloadMusicStream` |
| `LoadShader` | `UnloadShader` |
| `LoadRenderTexture` | `UnloadRenderTexture` |

Every `Load` must be matched by exactly one `Unload`. No garbage collector is coming.
Miss one and you leak GPU memory; call it twice and you free a handle someone else
is using.

And `Texture2D` is just a plain struct — an `unsigned int id` plus width, height,
mipmaps, format. Which leads directly to the trap:

## The trap that teaches you the language

```cpp
Texture2D a = LoadTexture("player.png");
Texture2D b = a;                            // copies the struct — and the id
// two objects, ONE GPU texture, and now two things believe they own it
```

Copying the struct copies the *handle*, not the resource. If both get unloaded,
you've freed the same GPU texture twice. If a wrapper class does this in a
destructor, you get a double-free that may not crash until much later, somewhere
unrelated. This is the single most valuable bug in this whole roadmap — it's the
concrete reason C++ has the ownership machinery it has.

## RAII: the idea

**Resource Acquisition Is Initialization.** Tie a resource's lifetime to an object's
scope: acquire in the constructor, release in the destructor. When the object goes
out of scope — normally, early-returned, or unwound by an exception — the destructor
runs and the resource is released. You cannot forget, because forgetting is no
longer possible.

This is the idea C++ is built around, and once it clicks, `unique_ptr`, `vector`,
`lock_guard` and `fstream` all stop looking like unrelated library types and start
looking like the same pattern applied to different resources.

## What to build

A `Texture` wrapper. Small, and it will teach you more than a week of reading:

1. Constructor loads. Destructor unloads. Verify with a print in each.
2. **Delete the copy constructor and copy assignment.** A GPU texture is not
   copyable — say so in the type system (`= delete`) and let the compiler reject the
   bug above at compile time rather than at 2am.
3. **Implement move constructor and move assignment.** Steal the handle, then null
   out the source's handle so its destructor becomes a no-op. Understanding *why the
   source must be left in a valid, destructible state* is the crux of move semantics.
   Get this and you understand `std::move`.
4. Now look up the **rule of five** and the **rule of zero**, and work out which one
   you just used and why the other doesn't apply here.
5. Then look at `unique_ptr` with a custom deleter and note it would have done most
   of this for you. Writing it by hand once first is still worth it.

## The raylib-specific gotcha that will bite you

**raylib resources need a live OpenGL context.** `LoadTexture` before `InitWindow`
fails silently. `UnloadTexture` after `CloseWindow` is undefined behaviour.

Now combine that with RAII: a `static` or global texture wrapper has a destructor
that runs at program exit — **after** `CloseWindow()` has already been called. You
get a crash or silent corruption on shutdown, with a stack trace pointing at nothing
you wrote.

This is a genuine, common, hard-to-diagnose bug, and it has a boring fix: don't put
graphics resources in globals or statics. Make ownership explicit and scoped inside
`main`, or hold them in a manager whose own lifetime you control and destroy
explicitly before `CloseWindow`. Decide your rule now and write it down.

## Then: a resource manager

Once the wrapper works, the next problem is that loading `player.png` from three
places loads it three times.

- Cache by path so repeated loads share one texture.
- Which means **shared** ownership, not unique — this is where `shared_ptr` earns
  its keep, and where you should be able to articulate the difference.
- Decide on eviction. Honestly, for your scale: load everything at scene enter,
  free at scene exit. Reference-counted unloading is a lot of machinery for a
  problem you probably don't have. Note it and move on.
- Handle the missing-file case. `LoadTexture` on a bad path returns a texture with
  `id == 0` rather than failing loudly. Decide: crash loudly in debug, or substitute
  a magenta placeholder? Silent failure is the worst option — pick one.
- Asset paths differ between running from the IDE and running the packaged binary,
  and differ again between macOS and Windows. Look up `GetApplicationDirectory` and
  `ChangeDirectory`. Chapter 14 goes deeper.

## Reading

- cppreference on rule of three/five/zero, and on move semantics.
- Scott Meyers, *Effective Modern C++*, items 17–25 — the definitive treatment of
  moves and perfect forwarding. Dense; read items 23 and 25 first.

## Definition of done

- [ ] A `Texture` wrapper that loads in its constructor and unloads in its destructor
- [ ] Copy is deleted; move is implemented and leaves the source destructible
- [ ] I can explain the rule of five and which parts I needed
- [ ] Loading the same file twice does not load it twice
- [ ] Missing files fail in a way I chose deliberately
- [ ] No graphics resource outlives `CloseWindow()` — and I know why that matters
- [ ] I verified no leaks (macOS: `leaks`; Windows: VS diagnostic tools)

Next: [11 — Entities and Data Layout](11-entities-and-data.md)
