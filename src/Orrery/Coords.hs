module Orrery.Coords
  ( eclipticToEquatorial
  , equatorialToHorizon
  ) where

import Orrery.Types
import Orrery.Time (JulianDay(..))

-- | Reduce angle to [0, 360)
normalise :: Double -> Double
normalise x = x - 360.0 * fromIntegral (floor (x / 360.0) :: Int)

-- | Mean obliquity of the ecliptic (degrees), Meeus Ch. 22
--   T = Julian centuries from J2000.0
obliquity :: Double -> Double
obliquity t = 23.4392911
            - 0.013004167  * t
            - 0.0000001638 * t * t
            + 0.0000005036 * t * t * t

-- | Ecliptic → Equatorial, Meeus Ch. 13, Eq. 13.3–13.4
eclipticToEquatorial :: JulianDay -> EclipticCoord -> EquatorialCoord
eclipticToEquatorial (JulianDay jd) (EclipticCoord (Degrees lon) (Degrees lat) _) =
  let t      = (jd - 2451545.0) / 36525.0
      eps    = obliquity t * pi / 180.0
      lam    = lon * pi / 180.0
      bet    = lat * pi / 180.0
      raRad  = atan2 (sin lam * cos eps - tan bet * sin eps) (cos lam)
      decRad = asin  (sin bet * cos eps + cos bet * sin eps * sin lam)
      raD    = normalise (raRad * 180.0 / pi)
      decD   = decRad * 180.0 / pi
  in  EquatorialCoord (Degrees raD) (Degrees decD)

-- | Equatorial → Horizon, Meeus Ch. 13, Eq. 13.5–13.6
--   gst: Greenwich Sidereal Time (degrees)
--   Azimuth is conventional: 0° = North, 90° = East
equatorialToHorizon :: Observer -> Degrees -> EquatorialCoord -> HorizonCoord
equatorialToHorizon (Observer (Degrees lat) (Degrees lon)) (Degrees gst)
                    (EquatorialCoord (Degrees raD) (Degrees decD)) =
  let lst    = normalise (gst + lon)           -- local sidereal time (°)
      h      = lst - raD                       -- local hour angle (°)
      hRad   = h   * pi / 180.0
      latRad = lat  * pi / 180.0
      decRad = decD * pi / 180.0
      -- Altitude
      sinAlt = sin latRad * sin decRad + cos latRad * cos decRad * cos hRad
      altRad = asin sinAlt
      -- Azimuth: Meeus formula gives angle from South westward;
      -- add 180° to get conventional North-based azimuth
      azRad  = atan2 (sin hRad) (cos hRad * sin latRad - tan decRad * cos latRad)
      azD    = normalise (azRad * 180.0 / pi + 180.0)
      altD   = altRad * 180.0 / pi
  in  HorizonCoord (Degrees altD) (Degrees azD)
