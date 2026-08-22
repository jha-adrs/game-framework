# Code Review Rubric

Run this after finishing a chapter, **before** ticking its Definition of done.

## How it works

1. **Self-review first.** Walk the checklist against your own code. Finding your own
   bugs is worth several times more than having them pointed out.
2. **Then ask for a review.** Say "review chapter N" and this rubric gets applied,
   plus anything specific to that chapter's **C++ you'll meet here** section.
3. **You write every fix.** The review names the defect, explains the concept, and
   points at the line. It does not hand over corrected code. That is the whole
   arrangement — see [00 — Start Here](00-start-here.md).

## Severity

| Tier | Meaning | Action |
|---|---|---|
| **1 — Undefined behaviour** | The standard gives no guarantee. May "work" today and break under a different compiler, platform, or optimisation level. | Fix now, always |
| **2 — Wrong semantics** | Well-defined but not what you meant. Silent no-ops, accidental copies, leaks. | Fix before moving on |
| **3 — Design** | Correct but poorly shaped. Wrong boundaries, misleading names, mixed responsibilities. | Discuss, fix if cheap |
| **4 — Polish** | Unused includes, formatting, stray tokens. | Batch them up |

Tier 1 first, always. A tidy program with UB in it is a program that works until it
doesn't.

## A. Undefined behaviour

- [ ] No numeric conversion where the value cannot fit the destination type
- [ ] No out-of-range arguments passed to library functions (**they do not validate**)
- [ ] No reading an uninitialised variable
- [ ] No dangling reference, pointer, or iterator
- [ ] No signed integer overflow
- [ ] Ran under `-fsanitize=address,undefined` and it was silent

> **Casts silence warnings; sanitizers do not.** An explicit cast tells the compiler
> to stop checking. That is why UBSan finds things `-Wall -Wextra` never will, and
> why both are worth having.

## B. Value semantics and references

- [ ] Anything you modify is passed by reference, not by value
- [ ] Anything large you only read is `const&`
- [ ] `for (auto& x : v)` where you mutate — the `&` is not optional
- [ ] No accidental copy from a bare `auto`
- [ ] Nothing returns a reference or pointer to a local

## C. Types and conversions

- [ ] `unsigned char` for byte-range values, never plain `char` (it is **signed** here)
- [ ] `static_cast`, not C-style casts
- [ ] Conversion happens **once**, as late as possible, to the type you actually want
- [ ] Rounding vs truncation was a decision, not an accident
- [ ] No integer division where float division was meant
- [ ] `size_t` vs `int` consistent in loops and comparisons

## D. `const` and interfaces

- [ ] `const` on every parameter you do not modify
- [ ] Function names describe what they return — a function called `getColor` returns
      a `Color`, not a channel
- [ ] One job per function
- [ ] No unused parameters, unused default arguments, or unused locals

## E. Lifetime and ownership *(from ch08 / ch10)*

- [ ] No graphics or audio resource in a global or `static` — its destructor runs
      after `CloseWindow()`
- [ ] Every `Load*` has exactly one matching `Unload*`
- [ ] Types owning a handle either delete copy or implement it correctly
- [ ] Any class with a `virtual` function has a `virtual` destructor

## F. Structure

- [ ] State that must persist lives **outside** the loop
- [ ] Computation happens **before** `BeginDrawing()`, not inside it
- [ ] No heap allocation per frame (`std::string`, `std::to_string`, `vector` growth)
- [ ] No I/O per frame (`std::cout`, file writes)
- [ ] Magic numbers are named `constexpr`

## G. Build hygiene

- [ ] Warning-clean under `-Wall -Wextra` (and `/W4` on MSVC)
- [ ] Include what you use — do not rely on transitive includes
- [ ] Dependencies pinned to a tag, and **verified**:
      `git -C build/_deps/<name> describe --all`
- [ ] No new compiler warnings introduced since the last chapter

## H. Cross-platform *(you have two machines — this is not hypothetical)*

- [ ] No `M_PI` (POSIX only; MSVC needs `_USE_MATH_DEFINES`)
- [ ] No plain `char` used as a byte value
- [ ] Nothing depending on `int` being a particular width
- [ ] Paths resolved relative to the executable, not the working directory
- [ ] Builds clean on both machines from the same commit

---

# Your recurring patterns

Not generic advice — these are bugs actually hit on this project, grouped by what
they have in common. This section grows as the project does, and it is the part
worth re-reading before starting a chapter.

## Pattern 1 — Range and endpoint errors *(hit 4×)*

| Bug | What happened |
|---|---|
| `* 100` on an already-correct range | Turned `0..256` into `0..25600`; every clean value became `0` after truncation |
| `128 - sin*127` | Mismatched constants gave `1..255` instead of `0..255` |
| `128 - sin*128` | Gave `0..256`; `256` does not fit a byte |
| truncating instead of rounding | Never reached `255`, because `sin` never lands on exactly `-1.0` at a sampled instant |

**The habit to build:** before running anything, hand-compute the output at both
**endpoints** and the midpoint. Four of these would have died in thirty seconds.

## Pattern 2 — Things that fail silently *(hit 3×)*

| Bug | Why nothing warned you |
|---|---|
| `GIT_TAB` instead of `GIT_TAG` | CMake does not validate keyword names — unknown keys are dropped, not rejected |
| `(char)` cast on an out-of-range value | An explicit cast switches the diagnostic **off** |
| `!WindowShouldClose` (no parens) | A function's address is always non-null, so the condition is simply always false |

**The habit to build:** when something is configured rather than computed, **verify
the effect, not the source**. `describe --all` told the truth that `CMakeLists.txt`
did not.

## Pattern 3 — Conversions done too early or through the wrong type *(hit 2×)*

`double → char → unsigned char` was UB in the middle step, and it *round-tripped
correctly at `-O0`* while producing `1834232120` at `-O2`. Same source, different
answer, no warning either time.

**The habit to build:** convert once, at the end, straight to the type you want.
Every intermediate type is a place for the value to be mangled.

## Pattern 4 — Trusting a C API to validate *(hit 1×)*

`ColorFromHSV(hue, 100, 1.0)` where saturation is documented `0..1`. raylib computed
`1 - 100k`, went negative, and cast a negative float to `unsigned char` — UB **inside
the library**, caused by your argument.

**The habit to build:** read the documented range in the header before the first call.
C libraries have no argument validation and no exceptions. They trust you completely.
