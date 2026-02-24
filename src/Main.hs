module Main where

import Orrery.Types
import Orrery.Time
import Orrery.FFI
import Orrery.Coords
import Orrery.Display

main :: IO ()
main = do
  -- Example: 2026-02-21 22:00:00 UTC, Stockholm
  let observer = Observer (Degrees 59.33) (Degrees 18.07)
      jd       = toJulianDay 2026 2 24 19 10 0
      sun      = sunPosition jd
      moon     = moonPosition jd
      gst      = toGreenwichSiderealTime jd
      moonEq   = eclipticToEquatorial jd moon
      moonHz   = equatorialToHorizon observer gst moonEq

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
  let (Degrees gstVal) = gst
  putStrLn $ "GST: " ++ show gstVal ++ "°"
  let EquatorialCoord (Degrees raVal) (Degrees decVal) = moonEq
  putStrLn $ "Moon RA: " ++ showDeg raVal ++ "  Dec: " ++ showDeg decVal
  putStrLn ""
  putStrLn $ formatHorizon moonHz
  putStrLn ""
  putStrLn "Rendering..."
  renderMoon sun moon 800 800 "moon.ppm"
  putStrLn "Done. Output: moon.ppm"
