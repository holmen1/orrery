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

  putStrLn "=== Project Moon ==="
  putStrLn ""
  putStrLn $ formatPosition "Sun " sun
  putStrLn $ formatPosition "Moon" moon
  putStrLn ""
  putStrLn $ formatHorizon moonHz
  putStrLn ""
  putStrLn "Rendering..."
  renderMoon sun moon 800 800 "moon.ppm"
  putStrLn "Done. Output: moon.ppm"
