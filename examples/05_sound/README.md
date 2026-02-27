# Example 05 — Sound

A character dances while music plays and returns to idle when it stops. Shows how to stream music, play one-shot sound
effects, and query audio state to drive behaviour.

## What this example shows

- **Music streaming** — loading and playing a looping `.ogg` track via `ctx.audio.play_music()` and
  `ctx.audio.stop_music()`; the engine streams the file rather than loading it fully into memory
- **Sound effects** — loading and playing a one-shot `.wav` sound effect via `ctx.audio.play_sound()`
- **Audio state query** — using `ctx.audio.is_music_playing()` to branch between play and stop without maintaining
  external state
- **Audio resource lifecycle** — explicit load in `init()` and unload in `end()` for both `music_handle` and
  `sound_handle`

## Key things to look at

- `sound.cpp` `update()` — the full play/stop toggle in one place: queries `is_music_playing()`, calls `play_music()`
  or `stop_music()` + `play_sound()`, and swaps the animation as a side effect
- `sound.cpp` `init()` / `end()` — shows the load/unload symmetry for music, sound, and animation library handles

## Controls

| Key / Button   | Action            |
|----------------|-------------------|
| `Space` / A    | Play / stop music |
| `F5` / START   | Toggle debug draw |
| `F11` / SELECT | Toggle fullscreen |
| `Esc` / B      | Exit              |