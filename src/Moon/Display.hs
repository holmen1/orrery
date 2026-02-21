module Moon.Display
  ( formatHorizon
  , formatPosition
  ) where

import Moon.Types

-- | Format horizon coords for the user
formatHorizon :: HorizonCoord -> String
formatHorizon (HorizonCoord (Degrees alt) (Degrees az)) =
  "Look at azimuth " ++ show (round az :: Int) ++ "°, altitude " ++ show (round alt :: Int) ++ "°"

-- | Format ecliptic position
formatPosition :: String -> EclipticCoord -> String
formatPosition name (EclipticCoord (Degrees lon) (Degrees lat) dist) =
  name ++ ": lon " ++ show lon ++ "°  lat " ++ show lat ++ "°  dist " ++ show (round dist :: Int) ++ " km"
