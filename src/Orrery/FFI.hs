module Orrery.FFI
  ( moonPosition
  , sunPosition
  , renderMoon
  ) where

import Orrery.Types (EclipticCoord(..), Degrees(..))
import Orrery.Time  (JulianDay(..))
import Foreign.Ptr (Ptr)
import Foreign.Marshal.Array (allocaArray, peekArray)
import System.IO.Unsafe (unsafePerformIO)

foreign import capi "ephemeris.h sun_position"
  c_sun_position :: Double -> Ptr Double -> IO ()

foreign import capi "ephemeris.h moon_position"
  c_moon_position :: Double -> Ptr Double -> IO ()

-- | Sun geocentric ecliptic position for a given Julian Day
sunPosition :: JulianDay -> EclipticCoord
sunPosition (JulianDay jd) = unsafePerformIO $
  allocaArray 3 $ \ptr -> do
    c_sun_position jd ptr
    [lon, lat, dist] <- peekArray 3 ptr
    return $ EclipticCoord (Degrees lon) (Degrees lat) dist

-- | Moon geocentric ecliptic position for a given Julian Day
--   TODO: wire to real implementation once moon_position is done
moonPosition :: JulianDay -> EclipticCoord
moonPosition (JulianDay jd) = unsafePerformIO $
  allocaArray 3 $ \ptr -> do
    c_moon_position jd ptr
    [lon, lat, dist] <- peekArray 3 ptr
    return $ EclipticCoord (Degrees lon) (Degrees lat) dist

-- | Placeholder: Render moon image
renderMoon :: EclipticCoord -> EclipticCoord -> Int -> Int -> FilePath -> IO ()
renderMoon _sun _moon _w _h _path = putStrLn "TODO: raytracer FFI"
