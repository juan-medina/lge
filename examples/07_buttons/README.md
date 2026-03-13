# Example 07 — Buttons

Shows interactive UI buttons with hover/pressed states, a modal popup panel, and controller overlay icons.

## What this example shows

- **Buttons** — `button` component with `normal_tint`, `hover_tint`, and `pressed_tint` for visual state feedback
- **Modal popup** — a panel that is hidden at startup, shown on demand, and hidden again after a choice is made
- **Hierarchy and hidden** — the popup panel's child buttons and label are automatically hidden with it via the `hidden`
  component
- **`button_clicked` event** — subscribing to button events to drive game logic without polling
- **Controller overlays** — `controller_button`, `overlay_sheet`, and `overlay_frame` fields on `button` display a
  controller icon at the bottom edge of the button when a controller is active; pressing the bound button fires
  `button_clicked` identically to a mouse click
- **Rich text labels** — result text uses `{#RRGGBB}` color tags to highlight which button was pressed

## Key things to look at

- `buttons.cpp` `init()` — panel and button setup, hierarchy attachment, `button_clicked` subscription
- `buttons.cpp` `show_popup()` / `hide_popup()` — toggling visibility across multiple entities and updating the result
  label
- `button` component fields — how controller bindings and overlay sprites are declared alongside visual tints

## Controls

| Key / Button     | Action            |
|------------------|-------------------|
| Click / Y        | Open popup        |
| Click Ok / A     | Confirm (Ok)      |
| Click Cancel / X | Dismiss (Cancel)  |
| `F5` / START     | Toggle debug draw |
| `F11` / SELECT   | Toggle fullscreen |
| `Esc` / B        | Exit              |