# 08 — Scenes and Lifecycle

**The problem:** in chapter 05 you did title/playing/gameover with an `enum` and a
`switch`. Add a pause menu, a settings screen and a level-select and that switch
becomes unreadable, every scene's variables live in one scope, and "reset the game"
means remembering which of forty variables belong to which screen.

**The fix:** make a scene a *thing* with a lifecycle, and give each one its own state.

## C++ you'll meet here

The biggest object-oriented C++ chapter. Much of this will look familiar from Java,
but the memory and lifetime rules are entirely different.

- **Inheritance and `virtual`** — `struct Scene` with virtual `enter/update/render/exit`.
  A virtual call dispatches on the *runtime* type through a vtable pointer.
- **`virtual` destructor** — **the critical one.** Deleting a derived object through a
  base pointer without a virtual destructor is undefined behaviour: the derived
  destructor never runs, so its resources leak. If a class has any virtual function,
  it needs a virtual destructor. No exceptions.
- **Pure virtual / abstract base** — `virtual void update(float) = 0;` makes `Scene`
  impossible to instantiate and forces every scene to implement it.
- **`override`** — always write it. It makes the compiler verify you're actually
  overriding, catching signature typos that would otherwise silently create a new
  unrelated function. A genuinely valuable keyword.
- **`std::unique_ptr<Scene>`** (`<memory>`) — owning, non-copyable, frees
  automatically. `std::vector<std::unique_ptr<Scene>>` is your scene stack.
  Note it can't be copied, only moved — which is chapter 10's topic arriving early.
- **`std::make_unique<TitleScene>(...)`** — how you construct one.
- **Slicing** — assigning a derived object to a base *value* silently truncates it.
  Polymorphism only works through references and pointers.
- **`std::variant`** (`<variant>`, C++17) — the non-virtual alternative, if you want
  to see the other approach.

Fuzzy on ownership? Read [10 — Resources and RAII](10-resources-and-raii.md) first;
this chapter leans on it.

## The shape

A scene needs, at minimum: enter, update, render, exit. In C++ that's a base class
with virtual functions, or a struct of function pointers, or a tagged union — see
the decisions below.

The manager owns the current scene, forwards update/render to it, and handles
transitions.

**Where it gets interesting:** a *stack*, not a single current scene. Pausing means
pushing a pause scene on top of the game scene — the game keeps its state, stops
updating, but is still *drawn underneath*. That single insight (update the top,
render the whole stack) buys you pause menus, modal dialogs, and inventory screens
for free. It's the reason to prefer a stack from the start.

## Decisions this forces on you

- **Polymorphism or not?** `virtual` functions are the obvious C++ answer and are
  completely fine at this scale. A tagged union / `std::variant` avoids heap
  allocation and virtual dispatch. Do not agonise — virtual is fine, and this is a
  good place to learn what a vtable actually is.
- **Who owns scenes?** `unique_ptr` in a `vector` is the boring correct answer.
  Chapter 10 is about ownership, so if this feels vague, do 10 first.
- **When does a transition happen?** Deleting the current scene *during* its own
  `update()` is a use-after-free waiting to happen. The usual fix is to queue the
  transition and apply it at a safe point in the frame. You will probably hit this
  bug — recognise it when you do.
- **Do scenes persist or rebuild?** Pausing must preserve state. Returning to the
  title screen probably shouldn't. Both need to be expressible.
- **Shared vs per-scene data.** Score belongs to the game scene. Settings and loaded
  assets don't. Drawing that line badly is how you get a global blob.

## Reading

- *Game Programming Patterns*, "State" chapter — covers exactly this, including the
  stack. Also "Subclass Sandbox" for what a scene base class should provide.

## Exercises

1. **The virtual destructor bug.** Give a base and a derived class destructors that
   print. Delete a derived object through a base pointer *without* `virtual` on the
   base destructor. Observe only one message. Add `virtual`. Observe both. Then run
   the broken version under ASan and read the leak report.
2. **`override` earns its keep.** Override `update(float dt)` but type the parameter
   as `double` by mistake. Without `override` it compiles and silently never gets
   called. With `override` it's a compile error. Do both.
3. **Slicing.** Assign a derived scene to a `Scene` *value* (not reference). Call a
   virtual function. Note which implementation runs, and why.
4. **The stack.** Implement push/pop, then a pause scene that renders the game
   underneath without updating it. Getting "update the top, render the whole stack"
   working is the payoff of the whole chapter.
5. **The transition bug.** Deliberately delete the current scene from inside its own
   `update()`. Run under ASan. Read the use-after-free. Then implement queued
   transitions.
6. **The other way.** Reimplement two scenes with `std::variant` plus `std::visit`
   instead of virtual functions. Decide which you prefer and be able to defend it.

## Definition of done

- [ ] Scenes are objects with enter/update/render/exit
- [ ] Transitions are queued, not applied mid-update
- [ ] A pause scene renders the game underneath without updating it
- [ ] Adding a new screen doesn't require touching a giant switch
- [ ] I wrote down what data is shared vs per-scene
- [ ] Reviewed against [the code review rubric](CODE-REVIEW.md) — tier 1 and 2 clear

Next: [09 — Input Mapping](09-input-mapping.md)
