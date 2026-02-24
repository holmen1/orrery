# Orrery 🌙

Predict where the Moon is and render what it looks like — Haskell + C.

## Build & Run

```
cabal run orrery
=== Orrery ===

Julian Day: 2461096.298611111
J2000 check: 2451545.0 (expect 2451545.0)

Sun : lon 336° 11' 15"  lat 0° 0' 0"  dist 148,061,725 km
Moon: lon 69° 51' 36"  lat 5° 16' 52"  dist 370,139 km

GST: 82.1729655507952°
Moon RA: 67.34°  Dec: 27.15°

Look at azimuth 230°, altitude 51°

Rendering...
TODO: raytracer FFI
Done. Output: moon.ppm
```

Requires GHC 9.6+ and a C compiler. That's it.

## What it does (so far)

See [PLAN.md](PLAN.md) for architecture, milestones, and progress.

## References

- **Jean Meeus, *Astronomical Algorithms*, 2nd ed.** — Willmann-Bell, 1998.
  The primary source for all ephemeris calculations in this project.
  - Ch. 7: Julian Day
  - Ch. 25: Solar Coordinates
  - Ch. 47: Position of the Moon
  - Available from [Willmann-Bell](http://www.willbell.com/math/mc1.htm) or [Amazon](https://www.amazon.com/Astronomical-Algorithms-Jean-Meeus/dp/0943396611)

- **USNO Astronomical Data Services** — https://aa.usno.navy.mil/
  Free online tools for verifying Sun and Moon positions against authoritative data.

- **IAU SOFA** — https://www.iausofa.org/
  The official C library implementing IAU astronomical standards.
  Well-commented source — useful as a cross-check on the Meeus algorithms.
