module Moon.FFI
  ( moonPosition
  , sunPosition
  , renderMoon
  ) where

import Moon.Types (EclipticCoord(..), Degrees(..))
import Moon.Time  (JulianDay(..))

-- TODO: FFI bindings to csrc/ephemeris.c and csrc/raytrace.c
--
-- foreign import ccall "moon_position" c_moon_position :: CDouble -> IO (Ptr EclipticCoord)
-- foreign import ccall "sun_position"  c_sun_position  :: CDouble -> IO (Ptr EclipticCoord)
-- foreign import ccall "render_moon"   c_render_moon   :: ... -> IO CInt

-- | Placeholder: Moon position for a given Julian Day
moonPosition :: JulianDay -> EclipticCoord
moonPosition (JulianDay _jd) =
  EclipticCoord (Degrees 0) (Degrees 0) 0

-- | Placeholder: Sun position for a given Julian Day
sunPosition :: JulianDay -> EclipticCoord
sunPosition (JulianDay _jd) =
  EclipticCoord (Degrees 0) (Degrees 0) 0

-- | Placeholder: Render moon image
renderMoon :: EclipticCoord -> EclipticCoord -> Int -> Int -> FilePath -> IO ()
renderMoon _sun _moon _w _h _path = putStrLn "TODO: raytracer FFI"
