# Example 05 — Sound

A character dances while music plays and returns to idle when it stops. Shows how to stream music, play one-shot sound
effects, query audio state to drive behaviour, and use mouse interaction to click on world entities.

## What this example shows

- **Music streaming** — loading and playing a looping `.ogg` track via `ctx.audio.play_music()` and
  `ctx.audio.stop_music()`; the engine streams the file rather than loading it fully into memory
- **Sound effects** — loading and playing a one-shot `.wav` sound effect via `ctx.audio.play_sound()`
- **Audio state query** — using `ctx.audio.is_music_playing()` to branch between play and stop without maintaining
  external state
- **Audio resource lifecycle** — explicit load in `init()` and unload in `end()` for both `music_handle` and
  `sound_handle`
- **Clickable entities** — opting the dancer into pointer interaction with the `clickable` tag; the engine's
  `pointer_system` fires a `lge::click` event when the entity is clicked
- **Hover feedback** — using the transient `lge::hovered` component to dim the sprite tint when the mouse is not over
  it, giving visual affordance that the entity is interactive; cursor changes to a hand on hover
- **Input routing** — the `lge::click` handler checks `c.entity` to route to the right response; hover tint is skipped
  when a controller is active since there is no pointer

## Key things to look at

- `sound.cpp` `init()` — how `clickable` is added to the sprite entity and how `ctx.events.on<lge::click>` subscribes
  to click events with entity identity check
- `sound.cpp` `update()` — how `lge::hovered` drives the sprite tint, and why the tint block is guarded by
  `all_of<lge::sprite>` (the animation system adds `sprite` lazily on the first frame)
- `sound.cpp` `toggle_music()` — the shared toggle logic called by both keypress and click paths

## Controls

| Key / Button       | Action                   |
|--------------------|--------------------------|
| `Space` / A        | Play / stop music        |
| Click dancer       | Play / stop music        |
| `F5` / START       | Toggle debug draw        |
| `F11` / SELECT     | Toggle fullscreen        |
| `Esc` / B          | Exit                     |