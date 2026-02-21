module Moon.Coords
  ( eclipticToEquatorial
  , equatorialToHorizon
  ) where

import Moon.Types

-- | Ecliptic → Equatorial, given obliquity of ecliptic
--   TODO: Meeus Ch. 13
eclipticToEquatorial :: EclipticCoord -> EquatorialCoord
eclipticToEquatorial _ecl =
  EquatorialCoord (Degrees 0) (Degrees 0)

-- | Equatorial → Horizon, given observer and sidereal time
--   TODO: Meeus Ch. 13
equatorialToHorizon :: Observer -> Double -> EquatorialCoord -> HorizonCoord
equatorialToHorizon _obs _siderealTime _eq =
  HorizonCoord (Degrees 0) (Degrees 0)
