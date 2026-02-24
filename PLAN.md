# Orrery 🌙

A mixed Haskell/C project to predict the position of the Moon (and Sun)
and render a raytraced visualization with correct illumination.

## Copilot Working Rules

- **Don't run ahead.** One step at a time. Wait for confirmation before moving on.
- **Show terminal output.** Let me see `cabal run` results — don't swallow them.
- **Keep code minimal.** No over-engineering, no unnecessary abstractions.
- **No bloat.** Every line earns its place.
- **Walking skeleton.** Every step must leave `cabal run` working.
- **Read this file** if chat history is lost — it has the full plan and progress.

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
- [x] C: Solar position (geocentric ecliptic lon/lat/dist)
- [x] C: Lunar position (geocentric ecliptic lon/lat/dist)
- [x] Haskell: Julian Day conversion
- [x] Haskell: FFI bindings to ephemeris
- [x] Haskell: Ecliptic → equatorial (RA/Dec)
- [x] Haskell: Equatorial → horizon (altitude/azimuth for observer lat/lon)
- [ ] Validate against known positions (e.g. USNO data)

### Step 5 — Raytracer (C + FFI)
- [x] C: Ray-sphere intersection
- [x] C: Illumination from Sun direction vector
- [x] C: PPM image output
- [x] Haskell: FFI bindings to raytracer
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

## Time Plan (Walking Skeleton)

Every step ends with `cabal run` producing correct output.
No step leaves the project in a broken state.

### Step 0 — Skeleton ✅
- Project structure, stubs, compiles and runs
- Output: zeroes everywhere, but it runs
- **Commit: "skeleton"**

### Step 1 — Julian Day (pure Haskell) ✅
- Implement `toJulianDay` in Moon.Time (Meeus Ch. 7)
- Main prints computed JD for hardcoded date
- Verify: 2000-01-01 12:00 UTC → JD 2451545.0
- `cabal run` → prints correct Julian Day
- Hurdles:
  - `ghc-pkg` not on PATH → fixed with `export PATH="/usr/lib/ghc-9.6.6/bin:$PATH"`
  - `Degrees(..)` constructor not exported from Types.hs → added `(..)` to exports
  - Type-defaults warnings on `fromIntegral` → added explicit `:: Double` annotations
- **Commit: "julian day"**

### Step 2 — Solar position (C + FFI) ✅
- Implement `sun_position()` in ephemeris.c (Meeus Ch. 25)
- Wire FFI in Orrery.FFI (foreign import ccall, marshalling)
- Main prints Sun ecliptic lon/lat/dist
- Verify: compare against USNO for a known date
- `cabal run` → prints Sun position

### Step 3 — Lunar position (C + FFI) ✅
- Implement `moon_position()` in ephemeris.c (Meeus Ch. 47, ~60 terms)
- Wire FFI (same pattern as Sun)
- Main prints both Sun and Moon positions
- Verify: compare Moon against USNO
- `cabal run` → prints Sun + Moon positions
- **Commit: "ephemeris"**

### Step 4 — Coordinate transforms (pure Haskell) ✅
- Implement ecliptic → equatorial in Moon.Coords
- Implement equatorial → horizon in Moon.Coords
- Implement sidereal time in Moon.Time
- Main prints "Look at azimuth X°, altitude Y°"
- Verify: compare alt/az against Stellarium or USNO
- `cabal run` → prints where to look in the sky
- **Commit: "coordinates"**

### Step 5 — Raytracer (C + FFI)
- Implement ray-sphere intersection in raytrace.c
- Implement Sun illumination (dot product shading)
- PPM output of lit Moon sphere
- Wire FFI, Main calls renderer after computing positions
- `cabal run` → prints position + writes moon.ppm with correct phase
- **Commit: "raytracer"**

### Step 6 — Polish
- CLI argument parsing (date + observer location)
- Earthshine, limb darkening
- Lunar surface texture (optional)
- `cabal run 2026-02-21T22:00 59.33 18.07` → full output
- **Commit: "v1.0"**

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
    └── Orrery/
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
