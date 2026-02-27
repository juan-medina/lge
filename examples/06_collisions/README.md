# Example 06 — Collisions

The most complete example. Shows collision detection, event-driven response, audio, and how to coordinate multiple
systems through events. Demonstrates that lge detects overlaps and fires events — all physics behaviour is implemented
in game code, not the engine.

## What this example shows

- **Collision detection** — opting entities into collision with the `collidable` tag component; the engine's
  `collision_system` fires a `lge::collision` event on the first frame two entities overlap, using SAT (Separating Axis
  Theorem) on oriented quads for accurate rotated collision
- **Event-driven architecture** — `lge::collision` is an engine event; the game translates it into a game-level
  `dice_hit` event carrying the entity and surface normal; systems and wall bounce both post `dice_hit`, so physics
  response and sound are handled in one place regardless of what was hit
- **Custom system** — `movement_system` handles per-entity physics (velocity, friction, wall bounce, rotation snap) and
  posts events rather than applying collision response directly
- **Physics response via reflection** — `on_dice_hit` uses the standard reflection formula `v - 2(v·n)n` to compute the
  new velocity from the surface normal, working identically for walls and dice
- **Sound** — loading and playing sound effects with cooldown throttling so rapid collisions don't saturate audio
- **Sprite sheets** — dice faces are frames in a sprite sheet; the face is randomised on collision
- **Debug draw** — F5 shows bounds quads in three states: default (no collidable), idle collidable, and currently
  overlapping

## Key things to look at

- `collisions.cpp` `on_collision()` — translates an engine collision into two `dice_hit` events with per-entity normals
  derived from relative positions
- `movement_system.cpp` `bounce_wall()` — posts `dice_hit` with axis-aligned wall normals; no physics response here
- `collisions.cpp` `on_dice_hit()` — the single place that applies the reflection formula and plays sound, regardless of
  whether the hit was a wall or another die
- `events.hpp` — the `dice_hit` struct: entity + normal is all the handler needs

## Controls

| Key / Button          | Action            |
|-----------------------|-------------------|
| `Space` / `Enter` / A | Throw dice        |
| `F5` / START          | Toggle debug draw |
| `F11` / SELECT        | Toggle fullscreen |
| `Esc` / B             | Exit              |