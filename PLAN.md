# Project Moon 🌙

A mixed Haskell/C project to predict the position of the Moon (and Sun)
and render a raytraced visualization with correct illumination.

## Architecture

```
  ┌─────────────────────────────────────────────────┐
  │                 Haskell (CLI)                    │
  │  Date + Location → orchestration → output        │
  │  • "Look at azimuth 135°, altitude 42°"          │
  │  • Raytraced PPM image of what you'd see         │
  └──────────┬──────────────────────┬───────────────┘
             │ FFI                  │ FFI
  ┌──────────▼──────────┐  ┌───────▼───────────────┐
  │   C: Ephemeris      │  │   C: Raytracer        │
  │                     │  │                        │
  │  • Moon position    │  │  • Ray-sphere intersect│
  │    (Meeus Ch.47)    │  │  • Sun illumination    │
  │  • Sun position     │  │  • Phase & terminator  │
  │    (Meeus Ch.25)    │  │  • PPM output          │
  └─────────────────────┘  └────────────────────────┘
```

## Language Split

| Component              | Language | Rationale                          |
|------------------------|----------|------------------------------------|
| Lunar periodic series  | C        | ~60 trig terms, tight loops        |
| Solar position         | C        | Same style, same coordinate system |
| Raytracer              | C        | Per-pixel math, performance        |
| Astronomical types     | Haskell  | Type safety, correctness           |
| Julian Day conversion  | Haskell  | Pure, testable date math           |
| Coordinate transforms  | Haskell  | Ecliptic → equatorial → horizon   |
| Observer & horizon     | Haskell  | Lat/lon + sidereal time → alt/az  |
| FFI bindings           | Haskell  | Bridge to both C components        |
| CLI & orchestration    | Haskell  | Composition, IO, user interface    |

## Milestones

### Phase 1 — Ephemeris
- [ ] C: Solar position (geocentric ecliptic lon/lat/dist)
- [ ] C: Lunar position (geocentric ecliptic lon/lat/dist)
- [ ] Haskell: Julian Day conversion
- [ ] Haskell: FFI bindings to ephemeris
- [ ] Haskell: Ecliptic → equatorial (RA/Dec)
- [ ] Haskell: Equatorial → horizon (altitude/azimuth for observer lat/lon)
- [ ] Validate against known positions (e.g. USNO data)

### Phase 2 — Raytracer
- [ ] C: Ray-sphere intersection
- [ ] C: Illumination from Sun direction vector
- [ ] C: PPM image output
- [ ] Haskell: FFI bindings to raytracer
- [ ] Render first Moon image with correct phase

### Phase 3 — Polish
- [ ] CLI: date + location → alt/az + image pipeline
- [ ] Earthshine (faint illumination on dark side)
- [ ] Lunar surface texture mapping (optional)
- [ ] Limb darkening

## End-to-End Flow

```
  Input:  2026-02-21 22:00 UTC,  59.33°N  18.07°E (Stockholm)
           │
           ▼
  Julian Day → C ephemeris → Sun & Moon ecliptic coords
           │
           ▼
  Ecliptic → Equatorial (RA/Dec) → Horizon (alt/az)
           │
           ├──→ "Look at azimuth 210°, altitude 35°"
           │
           ▼
  Sun–Moon geometry → C raytracer → moon.ppm
           │
           └──→ Image showing exactly the phase you'd see
```

Both outputs derive from the same Sun & Moon positions,
so the image and the pointing direction always agree.

## Time Plan

### Session 1 — Foundation
- Set up skeleton ✅
- Implement Julian Day conversion (Haskell, pure math)
- Implement solar position (C, Meeus Ch. 25, ~20 lines of trig)
- Verify Sun against a known date

### Session 2 — The Moon
- Implement lunar position (C, Meeus Ch. 47, the big one: ~60 terms)
- Wire up FFI for both Sun and Moon
- Verify Moon against known date

### Session 3 — Where to Look
- Ecliptic → equatorial transform
- Equatorial → horizon (observer lat/lon + sidereal time)
- CLI prints "look at azimuth X°, altitude Y°"

### Session 4 — Raytracer
- Ray-sphere intersection
- Sun illumination (dot product shading)
- PPM output — first rendered Moon!

### Session 5 — Integration & Polish
- Full pipeline: date + location → alt/az + image
- Validate against tonight's actual sky
- Earthshine, limb darkening if we feel ambitious

## Key References

- Jean Meeus, *Astronomical Algorithms*, 2nd ed.
  - Ch. 25: Solar Coordinates
  - Ch. 47: Position of the Moon
- ELP 2000-82 lunar theory (basis for Meeus's simplification)

## Project Structure

```
moon/
├── PLAN.md
├── moon.cabal
├── csrc/
│   ├── ephemeris.h       -- Sun & Moon position API
│   ├── ephemeris.c       -- Meeus periodic term summation
│   ├── raytrace.h        -- Raytracer API
│   └── raytrace.c        -- Sphere rendering with Sun illumination
└── src/
    ├── Main.hs           -- CLI entry point
    └── Moon/
        ├── Types.hs      -- Astronomical types (Angle, Coord, Body)
        ├── Time.hs       -- Date ↔ Julian Day
        ├── FFI.hs        -- Foreign imports (ephemeris + raytracer)
        └── Display.hs    -- Output formatting
```

## The Geometry

```
         Sun ☀
              \  sunlight
               \
                ● Moon
               /
              /  observer
         Earth 🌍
```

The raytracer places the camera at Earth, looking at the Moon.
For each point on the Moon's surface, the dot product of the
surface normal and the Sun direction vector determines brightness.
The crescent, half, gibbous, and full phases all emerge naturally
from this single geometric relationship.
