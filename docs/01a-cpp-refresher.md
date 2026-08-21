# 01a — C++ Refresher

An interlude, not a chapter. You know how to program; this is the C++-shaped subset
you need for the rest of the roadmap, aimed at someone fluent elsewhere who has
forgotten the details.

Read it once now. Come back to it whenever a later chapter's **C++ you'll meet here**
section names something that has gone fuzzy.

## Run these without CMake

Do the exercises as standalone console programs. No raylib, no build system, no
window — just a compiler and instant feedback:

```
clang++ -std=c++17 -Wall -Wextra ex01.cpp -o out/ex01 && ./out/ex01
```

Keep them in `exercises/`, compile into `exercises/out/` (gitignored). Turn the
warnings on from the start — `-Wall -Wextra` is where a lot of the actual teaching
happens, and getting used to a warning-clean build now is much easier than retrofitting
the habit later.

## 1. Value semantics — start here, it's the big one

In JS and Java, assigning an object copies a **reference**; both names see one object.
In C++, assigning a struct or class copies **the whole object** by default.

```cpp
struct Player { int hp; };
Player a{100};
Player b = a;     // a full copy. two independent Players.
b.hp = 50;        // a.hp is still 100
```

Everything else in this document is downstream of that one difference. Function
arguments copy. Returns copy. Putting something in a `vector` copies it. When someone
says C++ has "value semantics," this is what they mean, and it's the single most
common source of "why didn't my change stick?" for people arriving from managed
languages.

## 2. References and pointers

A **reference** (`T&`) is another name for an existing object. No copy, cannot be
null, cannot be reseated after binding.

```cpp
void heal(Player& p) { p.hp += 10; }   // modifies the caller's Player
void heal(Player  p) { p.hp += 10; }   // modifies a copy. does nothing useful.
```

`const T&` means "look, don't touch, don't copy" — the default choice for passing
anything bigger than a couple of machine words.

A **pointer** (`T*`) is an address. It can be null, can be reassigned, and needs `*`
to dereference / `->` for members. Rough guide for this project:

| Want | Use |
|---|---|
| Modify the caller's object | `T&` |
| Read a big object without copying | `const T&` |
| Small value (`int`, `float`, `Vector2`) | plain `T`, just copy it |
| "Maybe there is no object" | `T*` (null means none), or `std::optional<T>` |
| Own something heap-allocated | `std::unique_ptr<T>` — chapter 10 |

Raw `new` / `delete` should not appear in your code. If you think you need them,
chapter 10 has the answer you actually want.

## 3. `const`

`const` means "this cannot be modified," and the compiler enforces it. Read
declarations right-to-left:

```cpp
const int x = 5;          // int that cannot change
const Player& p;          // reference to a Player I promise not to modify
int getHp() const;        // member function that does not modify its object
```

Use it liberally. It documents intent, it catches real mistakes, and `const`
correctness is what makes chapter 15's tests easy to write. It is also viral in a
good way: a `const` function can only call other `const` functions.

## 4. Types worth being precise about

- **`int`** — 32-bit in practice. Fine for counts and indices.
- **`float`** vs **`double`** — raylib's API is `float` throughout. `double` is what
  `GetTime()` returns and what `<cmath>` prefers. Mixing them produces
  narrowing-conversion warnings; that's `-Wextra` doing its job, not noise.
- **`unsigned char`** — 0 to 255. This is what raylib's `Color` channels are, which
  is exactly why chapter 02's sin-wave exercise needs a deliberate conversion.
- **Unsigned wraparound.** `unsigned` types wrap instead of going negative:
  `(unsigned char)0 - 1` is `255`, silently. A real source of bugs in color and
  index math.
- **`size_t`** — the unsigned type `.size()` returns. Comparing it against a signed
  `int` in a loop is the classic `-Wsign-compare` warning.
- **`<cstdint>`** — `int32_t`, `uint8_t` etc. when you want a guaranteed width
  (file formats, save data, network).

## 5. `auto`

`auto` asks the compiler to deduce the type. Good for long types and iterators, bad
when it hides something you need to think about:

```cpp
auto i = 0;               // int
auto f = 0.0f;            // float
auto& p = players[0];     // REFERENCE — no copy
auto  q = players[0];     // COPY. easy to write by accident.
```

That last pair matters. `auto` without `&` copies.

## 6. Structs, and brace initialization

`struct` and `class` differ only in default access (`public` vs `private`). raylib is
a C library, so everything it gives you is a plain struct with public members and no
methods.

```cpp
struct Vec { float x; float y; };
Vec v{1.0f, 2.0f};        // brace init, in declaration order
Vec zero{};               // all members zero-initialized
Vec junk;                 // UNINITIALIZED. reading it is undefined behaviour.
```

That third line is a genuine trap with no equivalent in a managed language: local
variables are **not** zeroed for you. `-Wall` catches many cases; not all.

Prefer braces over parentheses — braces reject narrowing conversions, so
`unsigned char c{300}` is a compile error while the parenthesized version silently
truncates.

## 7. Functions

```cpp
float length(const Vec& v);           // declaration — what the compiler needs
float length(const Vec& v) { ... }    // definition — what the linker needs
```

That distinction is the same compile-vs-link split from chapter 01, at function
scope. It's why headers hold declarations and `.cpp` files hold definitions.

**Overloading** — same name, different parameter types — is fine and common.
**Default arguments** (`void draw(int x, int y, float scale = 1.0f)`) go in the
declaration only, never repeated in the definition.

## 8. Scope, lifetime, and the dangling-reference trap

A local variable dies at its closing brace. Returning a reference or pointer to one
is undefined behaviour:

```cpp
const Vec& bad() { Vec v{1,2}; return v; }   // v is gone. the reference dangles.
Vec good() { Vec v{1,2}; return v; }         // returns a copy. fine.
```

This is the class of bug AddressSanitizer exists to catch (chapter 15). It often
"works" in testing and breaks later, which is what makes it dangerous.

Rule of thumb for now: **return by value, pass by reference.** Copies of small
structs are cheap, and the compiler elides most of them anyway.

## 9. The three containers you need

- **`std::vector<T>`** (`<vector>`) — a growable array. Contiguous, cache-friendly,
  your default for "many things." `push_back`, `size()`, `[]`, `clear()`,
  `reserve()`, `empty()`.
- **`std::string`** (`<string>`) — text. `+` concatenates, `.c_str()` hands a
  `const char*` to C APIs like raylib's.
- **`std::array<T, N>`** (`<array>`) — fixed-size array that knows its own size.
  Better than `T arr[N]` because it doesn't decay to a pointer.

Two vector gotchas, both real:

```cpp
for (auto& p : players) p.hp--;         // reference: modifies in place
for (auto  p : players) p.hp--;         // COPY: modifies nothing. silent no-op.

players.push_back(x);                   // may REALLOCATE, invalidating
                                        // every pointer/reference/iterator into it
```

The second one is chapter 11's whole problem.

## 10. `enum class`

```cpp
enum class GameState { Title, Playing, GameOver };
GameState s = GameState::Playing;
```

Scoped and type-safe — it won't implicitly convert to `int`, so you can't accidentally
compare a `GameState` to a `Direction`. Use `enum class` over plain `enum` by default.
Converting deliberately needs `static_cast<int>(s)`, which chapter 09 uses to index
an array by action.

## 11. Casts

```cpp
static_cast<int>(someFloat)             // the one you want, ~99% of the time
reinterpret_cast<...>                   // bit-level reinterpretation. rare, dangerous.
(int)someFloat                          // C-style. works, but grep-hostile.
```

Prefer `static_cast` — it's searchable and it refuses conversions that are actually
unsafe.

## 12. Headers

```cpp
#pragma once        // top of every header. prevents double-inclusion.
```

Put **declarations** in `.h`, **definitions** in `.cpp`. Define a non-inline function
in a header, include it from two `.cpp` files, and the linker reports a duplicate
symbol — that's the One Definition Rule, and it's the most common self-inflicted link
error after forgetting `main`.

Chapter 06 is where this stops being theory, because that's when you split
`main.cpp` up.

## 13. `std::` and namespaces

`std::` is the standard library's namespace. Type it out. `using namespace std;` at
file scope drags thousands of names into scope and eventually collides with something
of yours — and it will do so in a confusing way.

## 14. Deliberately not now

You do not need these for the roadmap, and reaching for them early is a reliable way
to slow yourself down:

- **Templates** beyond *using* `vector<T>` — you'll want them around chapter 11 at
  the earliest.
- **Exceptions** — games typically don't use them for control flow. Know that RAII
  (chapter 10) is what makes exception safety work, and move on.
- **Smart pointers** — chapter 10 introduces them with a concrete reason.
- **Move semantics** — chapter 10, same.
- **`iostream`** — `std::cout` is fine for these exercises. In the game you have
  raylib's `TraceLog` and `DrawText`.
- Anything with "metaprogramming," "SFINAE," or "concepts" in the name.

## Exercises

Console programs. No raylib. The point of each is the *observation*, so predict the
output before running, then check.

1. **Copy semantics.** Make a `struct Player { int hp; std::string name; };`. Write
   `void hurt(Player p)` and `void hurtRef(Player& p)`, both subtracting 10 hp. Call
   each and print the result. Explain the difference out loud before you run it.

2. **The `auto` trap.** Fill a `std::vector<Player>` with three players. Loop with
   `for (auto p : v)` and set every `hp` to 0. Print the vector afterwards. Then
   change one character to fix it. This exact bug will cost you an hour some day;
   pay ten minutes now.

3. **Unsigned wraparound.** Declare `unsigned char c = 0;` then print `c - 1`, and
   also print `(unsigned char)(c - 1)`. Explain why they differ. Then try
   `unsigned char d{300};` and read the compiler error — that's brace init refusing
   a narrowing conversion.

4. **Uninitialized memory.** Declare a local `struct Vec { float x, y; };` *without*
   an initializer, print both members, and run it several times. Then compile with
   `-Wall -Wextra` and read the warning. Then add `{}` and observe the difference.

5. **Iterator invalidation.** Take a `std::vector<int>` with 3 elements. Store
   `int* p = &v[0];`, then `push_back` twenty times, then read `*p`. Run it under
   `-fsanitize=address`. Read what ASan tells you. This is chapter 11's core hazard,
   demonstrated in ten lines.

6. **ODR / linker error.** Make `util.h` with `int add(int a, int b) { return a + b; }`
   — a *definition* in a header. Include it from two `.cpp` files and link both.
   Read the error. Then fix it two different ways: `inline`, and moving the
   definition to `util.cpp`. Understand why both work.

7. **Dangling reference.** Write the `const Vec& bad()` function from section 8, call
   it, print the result. It may appear to work. Now run it under
   `-fsanitize=address,undefined` and see it caught. Notice how it "worked" — that's
   why undefined behaviour is dangerous rather than merely wrong.

8. **`enum class`.** Make `enum class Dir { Up, Down, Left, Right };` and write a
   function returning a `Vec` offset for each, using a `switch`. Then deliberately
   omit one case and read the `-Wswitch` warning. That warning is why `enum class`
   plus `switch` is a good pattern for chapter 05's game states.

## Definition of done

- [ ] All eight exercises written, run, and their output explained
- [ ] I can state the difference between `T`, `T&`, `const T&` and `T*` without looking
- [ ] I know why `for (auto x : v)` is usually wrong
- [ ] I have seen ASan catch a real bug
- [ ] I understand why a definition in a header can break the link
- [ ] `-Wall -Wextra` is on by default in my CMakeLists

Next: [02 — Window and Color](02-window-and-color.md)
