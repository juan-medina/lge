# Example 01 — Hello World

The entry point for understanding lge. Demonstrates the core building blocks you will use in every game.

## What this example shows

- **ECS basics** — creating entities and attaching components (`label`, `placement`)
- **Hierarchy** — parenting entities so children inherit transform and visibility from their parent
- **Custom systems** — writing and registering a `game_update` system (`oscillation_system`) that animates placement
  scale and rotation over time
- **Hidden component** — toggling visibility on a parent hides all its children; toggling on a child only hides that
  child
- **Actions** — binding keyboard keys and controller buttons to named actions and reading them in `update`
- **Debug draw** — pressing F5 shows bounds and pivot points for all entities
- **Rich text labels** — embedding color tags (`{#RRGGBB}`, `{#RRGGBBAA}`, `{#}`) directly in label strings to color
  individual words or characters; the `e` in "Hello" is green, the `l` in "World" is blue, and key names in the hint
  text are highlighted in yellow

## Key things to look at

- `src/hello_world.cpp` — entity setup, action handling, hierarchy construction
- `src/oscillation_system.hpp/cpp` — a minimal custom system, good template for writing your own
- **Rich text labels** — embedding color tags (`{#RRGGBB}`, `{#RRGGBBAA}`, `{#}`) directly in label strings to color
  individual words or characters; the `e` in "Hello" is green, the `l` in "World" is blue, and key names in the hint
  text are highlighted in yellow. Transparent by design — any `label` whose `text` contains `{#...}` tags is
  automatically rendered with per-segment colors, no extra components or setup needed.

## Controls

| Key / Button   | Action                                                          |
|----------------|-----------------------------------------------------------------|
| `1` / A        | Toggle hide Hello + World (hides parent → both children hidden) |
| `2` / X        | Toggle hide World only (hides child → parent stays visible)     |
| `F5` / START   | Toggle debug draw                                               |
| `F11` / SELECT | Toggle fullscreen                                               |
| `Esc` / B      | Exit                                                            |