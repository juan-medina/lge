# lge — Design Philosophy

This document explains *why* the engine is the way it is. The *how* is in `CLAUDE.md` and the source. This is for the moment six months from now when a decision looks weird and needs context.

---

## Resolution

Most engines fix a design resolution and scale it to the screen, adding black bars when the aspect ratio doesn't match. This is wrong for games that want to take advantage of the player's screen.

lge fixes the **height** of the design resolution and extends the **width** to fill whatever screen the player has. A wider screen shows more world horizontally. A narrower screen shows less. There are no black bars, no distortion, no wasted pixels.

This is how most modern 2D games actually ship — Terraria, Hollow Knight, and others all adapt horizontally — but most engines don't support it as a first-class feature. The tradeoff is that you cannot rely on a fixed canonical view of the world. UI elements and gameplay objects need to handle layout events when the viewport changes. This is a conscious design cost accepted in exchange for a better player experience.

lge is built for pixel art games. It uses a fixed design resolution that defines the logical pixel grid the game is authored against. The scene is rendered at this resolution and scaled up to fill the screen at integer or near-integer multiples, preserving the crispness of pixel art. Texture filtering is explicitly disabled engine-wide — bilinear filtering blurs pixel art and destroys the aesthetic it depends on. If the target game ever changes style, this is a one-line change, but the default is correct for the intended use case.

---

## Centered World Origin

The world origin is the center of the screen, not the top-left corner.

Top-left origins come from UI toolkits and early framebuffer conventions. They make sense for document layout. They are awkward for games, where most things are positioned relative to the center of the screen or relative to each other, not relative to a corner.

With a centered origin, placing something in the middle of the screen is `(0, 0)`. Symmetrical layouts are naturally symmetrical in code. There is no need for a camera system just to center the view — the world is already centered. This eliminates an entire category of coordinate conversion bugs.

The centered origin also makes the variable-width resolution model work without recalculation. Everything is drawn outward from the center. When the viewport widens, more world is revealed symmetrically on both sides. No coordinates need to be adjusted when the window size changes — the center is always the center, regardless of screen width.

---

## No Camera System

lge has no camera. This is intentional.

A camera system is needed when the world is larger than the screen and you need to scroll a viewport over it. For games with a fixed or semi-fixed view — turn-based games, arena games, games where the player is always roughly centered — a camera is an unnecessary layer of indirection that adds coordinate spaces to reason about.

If a future game genuinely needs scrolling, the right solution is to build it then, for that game, with the requirements in hand. Not to pre-build a general camera system that may not fit those requirements.

---

## ECS and Hierarchy

lge uses EnTT for entity-component-system architecture. The choice was made for three reasons.

First, ECS naturally separates data from logic, which keeps systems testable and composable. Second, EnTT is a mature, header-only library with excellent performance characteristics and no framework overhead. Third, the author had used ECS in a previous engine and understood the tradeoffs.

The cost is that ECS thinking is unfamiliar if you come from object-oriented game frameworks. Entities are IDs, not objects. Behavior lives in systems, not components. This takes adjustment but pays off as the codebase grows.

Plain ECS has no native concept of relationships between entities. lge adds a hierarchy system on top of EnTT because aggregation is fundamental to how games are actually built. Characters and enemies are frequently composed from parts — a body, a weapon, a visual effect — that need to move together, inherit visibility, and share transform state. Without a hierarchy, this composition has to be managed manually in every system that touches those entities, which is error-prone and repetitive.

The hierarchy system handles parent-child relationships, transform propagation, and inherited state (visibility, render order) automatically. Game code composes entities from parts and the engine ensures they behave as a unit.

---

## Error Handling

No exceptions. No raw error codes. All fallible operations return `lge::result<T>`, a variant-based type similar to C++23 `std::expected`.

Exceptions were ruled out because they make control flow invisible and interact badly with hot-path code. Raw error codes were ruled out because they are easy to ignore silently. `result<T>` forces the caller to acknowledge failure and makes propagation explicit with minimal boilerplate.

Errors carry stack traces via `lge::error`. They propagate up and are logged at the top of the call stack. Logging is never used as a substitute for error propagation — if something failed, return the failure, don't just log it and continue.

---

## Phase-Based Systems

Systems are registered to phases: `game_update`, `local_update`, `global_update`, `render`, `post_render`. This is not a general scheduler — it is a fixed sequence that reflects how a 2D game frame actually works.

The phases exist because different systems have different ordering requirements. Transform propagation must happen after game logic but before rendering. Render order must be resolved before the draw calls. Making the phases explicit and named eliminates the category of bugs caused by systems running in the wrong order.

---

## Backend Abstraction

Renderer, Input, and ResourceManager are interfaces. The raylib implementations are hidden behind them and not exposed to game code.

This is not about making raylib swappable for another backend — that is unlikely to ever happen. It is about keeping game code honest. Code that depends only on the interface is easier to read, easier to test, and does not accidentally reach into raylib internals that could change.

---

## What This Engine Is Not

lge does not have a scene editor, a scripting language, a networking layer, a physics engine, or a particle system. These are not planned features waiting to be built. They are deliberate non-features.

Every general-purpose system added to an engine is a system that must be designed, built, maintained, and worked around when it doesn't fit the game being made. lge follows YAGNI strictly: features are added when a concrete game requires them, with the requirements of that game in hand, not speculatively.

If you need something lge does not have, build it in the game, not in the engine — unless it is clearly general enough to belong here and you have a real use case driving the design.