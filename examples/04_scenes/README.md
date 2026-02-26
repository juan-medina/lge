# Example 04 — Scenes

Shows how to structure a game with multiple scenes and transition between them.

## What this example shows

- **Scene manager** — registering scenes with `scenes.add<T>()` and activating them with `activate<T>()` or `transition_activate<T>()`
- **Scene lifecycle** — each scene implements `on_enter`, `on_exit`, `on_pause`, `on_resume`; the manager calls these automatically during transitions
- **Fade transitions** — `transition_activate` fades the screen out, swaps the scene, then fades back in; the incoming scene starts paused and resumes when fully visible
- **Event-driven scene switching** — scenes post events (`go_to_game`, `go_to_menu`) without knowing what happens next; the app wires those events to scene transitions
- **Passing data between scenes** — `go_to_game` carries a `type` field that is forwarded to `game_scene::on_enter`, showing how to pass context from one scene to the next
- **Scene-owned entities** — entities created inside a scene are automatically destroyed when the scene exits via `clear_owned()`

## Key things to look at

- `scenes_game.cpp` — the app registers scenes and wires events to transitions; this is the only place that knows about both scenes
- `scenes/menu_scene.cpp` and `scenes/game_scene.cpp` — each scene only knows its own logic and posts events when something happens
- `events.hpp` — the shared event contract between scenes and the app

## Controls

Vary by scene — follow the on-screen prompts.

| Key / Button | Action |
|---|---|
| `F5` / START | Toggle debug draw |
| `F11` / SELECT | Toggle fullscreen |
| `Esc` / B | Exit |