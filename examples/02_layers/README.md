# Example 02 — Layers

Shows how lge controls render order using the `order` component and hierarchy.

## What this example shows

- **Shapes** — creating `rect` and `circle` entities with fill and border colors
- **Render order** — the `order` component controls which entities draw on top; higher layer = drawn last = appears in front
- **Hierarchy and order** — entities are grouped under color-coded root entities; changing a root's layer brings its entire group to the front
- **Dynamic layer changes** — modifying `order.layer` at runtime immediately affects draw order next frame

## Key things to look at

- `layers.cpp` — `create_root()` shows the minimal setup for a layered group; `change_top_color()` shows how to change layer at runtime
- The three root entities (`red_root_`, `green_root_`, `blue_root_`) each own a group of shapes as children — changing the root's layer lifts the whole group

## Controls

| Key / Button | Action |
|---|---|
| `1` / A | Cycle top color (none → red → green → blue → none) |
| `F5` / START | Toggle debug draw |
| `F11` / SELECT | Toggle fullscreen |
| `Esc` / B | Exit |