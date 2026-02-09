# Engine TODO

## Systems

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
