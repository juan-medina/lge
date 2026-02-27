# Examples

Each example is a self-contained application demonstrating specific lge engine concepts. They are numbered in order of
complexity — start from the top if you are new to lge.

All examples share a common base (`examples/src/example`) that handles the window, input hints, and common controls (F5
debug draw, F11 fullscreen, Esc exit).

---

## [01 — Hello World](01_hello_world/)

The entry point for lge. Covers the core building blocks used in every game: creating entities, attaching components,
parenting entities in a hierarchy, writing a custom system, and toggling visibility.

**Concepts:** ECS basics, hierarchy, custom systems, hidden component, actions

---

## [02 — Layers](02_layers/)

Shows how render order works. Hundreds of randomly placed shapes are grouped by colour; pressing a key brings one
colour's entire group to the front by changing a single layer value on the root entity.

**Concepts:** `order` component, render layers, hierarchy and inherited order, dynamic layer changes

---

## [03 — Sprites](03_sprites/)

A scrolling scene with an animated character and a multi-layer parallax background. Shows how to load sprite sheets and
animation libraries, use pivot points, and build a seamless scrolling background.

**Concepts:** sprite sheets, sprite animation, pivot points, parallax scrolling, custom components

---

## [04 — Scenes](04_scenes/)

A mini game loop with a menu and a game scene. Shows how to register scenes, switch between them with fade transitions,
and pass data from one scene to the next — all coordinated through events so scenes stay decoupled from each other.

**Concepts:** scene manager, scene lifecycle, fade transitions, event-driven scene switching, data passing between
scenes

---

## [05 — Sound](05_sound/)

A character dances while music plays and returns to idle when it stops. Shows how to stream music, play one-shot sound
effects, and query audio state to drive behaviour.

**Concepts:** music streaming, sound effects, audio state query, audio resource lifecycle

---

## [06 — Collisions](06_collisions/)

The most complete example. Dice are thrown onto a board and collide with each other and the walls. Shows collision
detection and how to encapsulate all physics, audio, and roll tracking inside a single self-contained system. The game
posts one event to throw and receives one event with the result — the system owns everything in between.

**Concepts:** `collidable` component, collision events, SAT overlap detection, game-side physics response, reflection
formula, self-contained systems, system lifecycle, event-driven input/output, roll tracking, sound, debug draw for
collisions