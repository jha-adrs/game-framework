# 16 — Shipping and Retrospective

## C++ you'll meet here

Little new language material — this chapter is mostly toolchain and judgement. Two
things worth knowing:

- **`NDEBUG`** — defined automatically in Release. It removes every `assert`, which
  means a Release build genuinely executes different code from the Debug build you
  tested. Test the Release build before shipping it, not just Debug.
- **`WIN32_EXECUTABLE`** — flipping it changes your entry point from `main` to
  `WinMain`, which is a linker-level change. Keep it off for Debug so you keep a
  console for output.

The retrospective is the real content here. Two questions to add to it, now that
you've been writing C++ for a while:

- **Which C++ features did I use because they fit, and which because I'd just learned
  them?** Template and inheritance regret is common and worth naming.
- **Where am I still guessing?** Whatever's on that list is your next reading, and
  it's a better-informed list than any curriculum could have given you upfront.

## Part A — Get it onto someone else's machine

A build a stranger can double-click is a different artifact from one that runs on
your dev box, and the gap is where the last unpleasant surprises live.

### Windows

Easiest platform to ship on. Two things to fix:

1. **Hide the console window.** By default you get a black terminal behind your game.
   In CMake: `set_target_properties(game PROPERTIES WIN32_EXECUTABLE TRUE)`. Note
   this changes the entry point to `WinMain`, so keep it **off** for debug builds —
   you want `printf` going somewhere visible while developing.
2. **Ship assets next to the exe.** If chapter 14's asset-copy step works, this is
   already done. A zip containing `game.exe` + `assets/` is a completely legitimate
   Windows release.

SmartScreen will warn on an unsigned exe. Code signing certificates cost real money
per year; for a hobby project, don't. Just tell people.

### macOS

Harder, and worth knowing why before you start.

1. **A bare binary works for you but not for others** — Gatekeeper blocks unsigned
   downloaded binaries outright.
2. **`.app` bundle.** A directory with a specific structure (`Contents/MacOS/`,
   `Contents/Resources/`, `Info.plist`). CMake can build one:
   `set_target_properties(game PROPERTIES MACOSX_BUNDLE TRUE)`. Note your working
   directory inside a bundle is *not* where you'd guess — chapter 14's path handling
   is what saves you here.
3. **Signing and notarisation.** To distribute without users right-clicking →
   Open and clicking through a scary dialog, you need an Apple Developer account
   ($99/year), a Developer ID certificate, `codesign`, and submission to Apple's
   notary service. This is a genuine, tedious, well-documented pain.

**My honest recommendation:** don't notarise a hobby game. Ship the Windows zip,
ship a macOS zip with a one-line "right-click → Open the first time" note, and spend
the $99 on something else. Revisit only if you ever actually sell something.

### Where to put it

**[itch.io](https://itch.io)** is the answer for hobby games. Free, expects zips,
per-platform uploads, no gatekeeping, and its `butler` CLI makes uploading a single
command you can script. GitHub Releases also works fine if your audience is
technical.

### Optional: automate it

You have `workflow` scope on your GitHub token. A GitHub Actions matrix build across
`macos-latest` and `windows-latest` that builds and uploads artifacts on tag is a
genuinely satisfying afternoon, and it also means **CI catches cross-platform
breakage without you switching machines** — which is the real payoff, more than the
release artifacts.

## Part B — The retrospective (the actual point of this chapter)

You now have a game, and a pile of code you extracted while making it. Sit down and
answer these honestly, in writing, committed to the repo.

**1. What is my engine, actually?**
List the files you'd copy into a brand new project verbatim. That list — not your
aspirations — is your engine. It is probably smaller than you expected, and that is
a good sign, not a bad one.

**2. What did I abstract that I shouldn't have?**
Look for interfaces with exactly one implementation, config that's never been
changed from its default, and "flexibility" nothing has used. Delete it. Deleting
speculative abstraction is one of the most satisfying and instructive things you can
do — and being willing to do it is what keeps a codebase alive.

**3. What did I not abstract that I should have?**
Where does the same change still require edits in three files?

**4. What do I still not understand?**
Be specific. "Templates" is not an answer; "why my move assignment operator needs to
handle self-assignment" is. This list is your next reading list.

**5. What was the most fun?**
Genuinely the most important question here. Rendering? Systems architecture?
Gameplay feel? Tools? Shaders? You started this with no goal, and the honest answer
to this question **is** the goal. Follow it.

## Where to go next

Pick based on question 5:

- **Loved gameplay/feel** → make more small games. Don't touch the engine. Read up
  on game feel and juice; watch a GDC talk or two on the subject.
- **Loved architecture** → this is where an ECS finally makes sense. Read EnTT's
  source. Consider a data-driven level format and a tools pipeline.
- **Loved rendering** → shaders, lighting, particles, then a step down to raw OpenGL
  or Vulkan to see what raylib was doing for you. Big, rewarding rabbit hole.
- **Loved the low-level C++** → custom allocators, arena allocation, cache
  optimisation, SIMD. Read about data-oriented design.
- **Loved tools** → level editors, hot reloading, asset pipelines. Underrated,
  genuinely valuable, and few hobbyists go here.

## Exercises

1. **Ship the Windows zip.** `game.exe` plus `assets/`, console hidden in Release.
   Have someone who is not you download and run it.
2. **Ship the macOS zip.** With the right-click-to-open note. Confirm your asset
   paths survive being inside a `.app` bundle — chapter 14's path work is what makes
   this pass or fail.
3. **Test the Release build properly.** Play through your whole game in Release, not
   Debug. Asserts are gone; anything you were relying on them for is now silent.
4. **Optional: CI.** A GitHub Actions matrix over `macos-latest` and
   `windows-latest`. The real value is catching cross-platform breakage without
   switching machines.
5. **The retrospective.** All five questions in Part B, plus the two C++ questions
   above, written down and committed. This is the actual final exercise of the
   roadmap.

## Definition of done

- [ ] A Windows zip a friend can run
- [ ] A macOS zip a friend can run (with instructions)
- [ ] Retrospective answers written down and committed
- [ ] I know what my engine actually is, as a file list
- [ ] I deleted at least one abstraction I didn't need
- [ ] I know what I want to do next, and it's based on what was fun
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

---

Back to [00 — Start Here](00-start-here.md)
