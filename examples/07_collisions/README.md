# Example 07 — Collisions

The most complete example. Dice are thrown onto a board and collide with each other and the walls. Shows collision
detection, event-driven physics fully encapsulated in a system, audio lifecycle, and a clean separation between game
input and game logic.

## What this example shows

- **Collision detection** — opting entities into collision with the `collidable` tag component; the engine's
  `collision_system` fires a `lge::collision` event on the first frame two entities overlap, using SAT (Separating Axis
  Theorem) on oriented quads for accurate rotated collision
- **Self-contained system** — `dice_roller_system` owns everything about how dice behave: spawning, physics (velocity,
  friction, wall bounce, rotation snap), collision response, audio, face randomisation, and roll tracking; the game
  knows nothing about any of it
- **Event-driven architecture** — the game posts `dice_throw{.count = N}` and eventually receives `dice_roll_result`
  with the settled values and total; all logic in between is the system's concern
- **System lifecycle** — `init()` loads resources and subscribes to events; `end()` unloads them; the game only
  registers the system
- **Physics response via reflection** — `apply_bounce` uses the standard reflection formula `v - 2(v·n)n` with damping
  and boost, applied identically for wall hits and dice-to-dice collisions
- **Roll tracking** — `dice_in_flight_` counts active dice; each die sets `settled = true` when it stops; if a settled
  die is knocked it re-enters flight and its value is removed from the settled list; `dice_roll_result` is only posted
  when the counter reaches zero
- **Sound** — throw sound and hit sounds are all owned by the system; hit sounds use cooldown throttling to avoid
  audio saturation on rapid collisions
- **Sprite sheets** — dice faces are frames in a sprite sheet; the face is always randomised to a different value on
  every bounce for clear visual feedback
- **Debug draw** — F5 shows bounds quads in three states: default (no collidable), idle collidable, and currently
  overlapping

## Key things to look at

- `dice_roller_system.cpp` `on_dice_throw()` — resets all state, destroys old dice, spawns new ones with staggered
  delays
- `dice_roller_system.cpp` `apply_bounce()` — single function handling reflection, face randomisation, re-entering
  flight for settled dice, and sound; called for both wall and dice-to-dice hits
- `dice_roller_system.cpp` `snap_to_rest()` — marks a die settled, records its value, decrements the counter, and
  calls `check_roll_complete()`
- `dice_roller_system.cpp` `check_roll_complete()` — posts `dice_roll_result` only when all dice have settled
- `collisions.cpp` — the entire game: bind action → post `dice_throw` → receive `dice_roll_result`; no physics, no
  audio, no entity management

## Controls

| Key / Button          | Action            |
|-----------------------|-------------------|
| `Space` / `Enter` / A | Throw dice        |
| `F5` / START          | Toggle debug draw |
| `F11` / SELECT        | Toggle fullscreen |
| `Esc` / B             | Exit              |