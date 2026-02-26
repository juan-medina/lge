# Example 03 — Sprites

Shows sprite sheets, sprite animation, and a parallax scrolling background built from layered sprites.

## What this example shows

- **Sprite sheets** — loading a JSON sprite sheet and assigning frames to entities via `sprite` component
- **Sprite animation** — using `sprite_animation` with an animation library to switch between named animations (idle, run)
- **Pivot points** — the character uses `pivot::bottom_center` so it sits naturally on the ground line regardless of frame size
- **Parallax scrolling** — multiple background layers move at different speeds relative to player input, creating depth
- **Seamless wrapping** — each background layer is duplicated (left + center + right) and wraps around so scrolling never ends
- **Custom component** — `parallax` is a simple game-side component holding a scroll factor, read by the update loop

## Key things to look at

- `sprites.cpp` `init()` — how sprite sheets and animation libraries are loaded and how background layers are created in a loop with factors
- `sprites.cpp` `update()` — how player input drives both the animation switch and the background scroll
- The `parallax` struct in `sprites.hpp` — an example of a minimal game-specific component

## Controls

| Key / Button | Action |
|---|---|
| `←` / `A` / `O` / D-pad left | Move left |
| `→` / `D` / `P` / D-pad right | Move right |
| `F5` / START | Toggle debug draw |
| `F11` / SELECT | Toggle fullscreen |
| `Esc` / B | Exit |