# Engine TODO

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
