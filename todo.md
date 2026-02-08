# Engine TODO

## Components

### placement (game-authored)
```cpp
struct placement {
    glm::vec2 position;  // local position relative to parent
    float rotation;      // local rotation
    glm::vec2 scale;     // local scale
};
```
- Represents where the entity is, how it’s rotated, and how big it is (relative to parent).

### transform (engine-derived, world-space)
```cpp
struct transform {
    glm::vec2 position;  // world-space position
    float rotation;      // world-space rotation
    glm::vec2 scale;     // world-space scale
};
```

- Computed world-space placement after hierarchy.

### metrics (engine-derived, render-space)
```cpp
struct metrics {
    glm::vec2 size;         // pixel size of sprite or text
    glm::vec2 pivot_offset; // pixel offset from pivot
};

```
- Computed from sprite/label data.

### bounds (engine-derived, rotated bounding shape)
```cpp
struct bounds {
    glm::vec2 p0;  // top-left corner (rotated)
    glm::vec2 p1;  // top-right corner (rotated)
    glm::vec2 p2;  // bottom-right corner (rotated)
    glm::vec2 p3;  // bottom-left corner (rotated)
};

```


- Used for debug drawing and mouse picking.

---

## Systems

### MetricsSystem (LOCAL)
Inputs:
- sprite or label
- resource manager
- placement.scale (optional if scale affects text measurement)

Outputs:
- metrics

Responsibilities:
- Compute pixel size (sprite frame or text measurement).
- Compute pivot offset.
- Produces local-space values only.

### TransformSystem (WORLD)
Inputs:
- placement
- parent transform (optional)

Outputs:
- transform

Responsibilities:
- Compute world-space position, rotation, scale.
- Apply hierarchy.
- Handle dirty propagation.

### BoundsSystem (WORLD GEOMETRY)
Inputs:
- transform
- metrics

Outputs:
- bounds

Responsibilities:
- Compute scaled size = metrics.size * transform.scale
- Compute scaled pivot = metrics.pivot_offset * transform.scale
- Build the 4 corners of the rectangle
- Rotate corners around world position
- Store rotated world-space bounding shape

### RenderSystem
Inputs:
- transform
- metrics
- sprite or label

Responsibilities:
- Compute final draw position = transform.position + (metrics.pivot_offset * transform.scale)
- Draw sprite or label
- Optionally draw bounds for debug

## Pipeline Order

1. MetricsSystem → compute local size + local pivot
2. TransformSystem → compute world-space transform
3. BoundsSystem → compute world-space rotated bounding shape
4. RenderSystem → draw
