module Orrery.FFI
  ( moonPosition
  , sunPosition
  , renderMoon
  ) where

import Orrery.Types (EclipticCoord(..), Degrees(..))
import Orrery.Time  (JulianDay(..))
import Foreign.Ptr (Ptr)
import Foreign.C.Types (CInt(..))
import Foreign.C.String (CString, withCString)
import Foreign.Marshal.Array (allocaArray, peekArray)
import System.IO.Unsafe (unsafePerformIO)

foreign import capi "ephemeris.h sun_position"
  c_sun_position :: Double -> Ptr Double -> IO ()

foreign import capi "ephemeris.h moon_position"
  c_moon_position :: Double -> Ptr Double -> IO ()

foreign import capi "raytrace.h render_moon"
  c_render_moon :: Double -> Double -> Double   -- sun  lon/lat/dist
               -> Double -> Double -> Double    -- moon lon/lat/dist
               -> CInt -> CInt                  -- width height
               -> CString                       -- output_path
               -> IO CInt

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

-- | Render a PPM image of the Moon lit by the Sun.
renderMoon :: EclipticCoord -> EclipticCoord -> Int -> Int -> FilePath -> IO ()
renderMoon sun moon w h path =
  let EclipticCoord (Degrees sl) (Degrees sb) sd = sun
      EclipticCoord (Degrees ml) (Degrees mb) md = moon
  in withCString path $ \cpath -> do
       rc <- c_render_moon sl sb sd ml mb md
                            (fromIntegral w) (fromIntegral h) cpath
       if rc /= 0
         then putStrLn $ "Warning: render_moon returned " ++ show rc
         else return ()
