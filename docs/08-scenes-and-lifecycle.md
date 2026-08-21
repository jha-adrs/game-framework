# 08 — Scenes and Lifecycle

**The problem:** in chapter 05 you did title/playing/gameover with an `enum` and a
`switch`. Add a pause menu, a settings screen and a level-select and that switch
becomes unreadable, every scene's variables live in one scope, and "reset the game"
means remembering which of forty variables belong to which screen.

**The fix:** make a scene a *thing* with a lifecycle, and give each one its own state.

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

## Definition of done

- [ ] Scenes are objects with enter/update/render/exit
- [ ] Transitions are queued, not applied mid-update
- [ ] A pause scene renders the game underneath without updating it
- [ ] Adding a new screen doesn't require touching a giant switch
- [ ] I wrote down what data is shared vs per-scene

Next: [09 — Input Mapping](09-input-mapping.md)
