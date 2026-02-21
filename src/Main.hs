module Main where

import Moon.Types
import Moon.Time
import Moon.FFI
import Moon.Coords
import Moon.Display

main :: IO ()
main = do
  -- Example: 2026-02-21 22:00:00 UTC, Stockholm
  let observer = Observer (Degrees 59.33) (Degrees 18.07)
      jd       = toJulianDay 2026 2 21 22 0 0
      sun      = sunPosition jd
      moon     = moonPosition jd
      moonEq   = eclipticToEquatorial moon
      moonHz   = equatorialToHorizon observer 0.0 moonEq

  putStrLn "=== Orrery ==="
  putStrLn ""

  -- Step 1: verify Julian Day
  let (JulianDay jdVal) = jd
  putStrLn $ "Julian Day: " ++ show jdVal
  -- Sanity check: 2000-01-01 12:00 UTC = JD 2451545.0
  let (JulianDay j2k) = toJulianDay 2000 1 1 12 0 0
  putStrLn $ "J2000 check: " ++ show j2k ++ " (expect 2451545.0)"
  putStrLn ""
  putStrLn $ formatPosition "Sun " sun
  putStrLn $ formatPosition "Moon" moon
  putStrLn ""
  putStrLn $ formatHorizon moonHz
  putStrLn ""
  putStrLn "Rendering..."
  renderMoon sun moon 800 800 "moon.ppm"
  putStrLn "Done. Output: moon.ppm"
