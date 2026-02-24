module Orrery.Display
  ( formatHorizon
  , formatPosition
  , showDeg
  ) where

import Orrery.Types

-- | Format horizon coords for the user
formatHorizon :: HorizonCoord -> String
formatHorizon (HorizonCoord (Degrees alt) (Degrees az)) =
  "Look at azimuth " ++ show (round az :: Int) ++ "°, altitude " ++ show (round alt :: Int) ++ "°"

-- | Format decimal degrees compactly (2 decimal places)
showDeg :: Double -> String
showDeg d = show (fromIntegral (round (d * 100) :: Int) / 100.0 :: Double) ++ "°"

-- | Format decimal degrees as d° m' s"
showDMS :: Double -> String
showDMS dd =
  let absDd   = abs dd
      d       = floor absDd :: Int
      remMin  = (absDd - fromIntegral d) * 60.0
      m       = floor remMin :: Int
      s       = (remMin - fromIntegral m) * 60.0
      sInt    = round s :: Int
      sign    = if dd < 0 then "-" else ""
  in  sign ++ show d ++ "° " ++ show m ++ "' " ++ show sInt ++ "\""

-- | Format an integer with comma thousands separators
showComma :: Int -> String
showComma n
  | n < 0     = "-" ++ showComma (negate n)
  | otherwise = reverse . insertCommas . reverse $ show n
  where
    insertCommas []       = []
    insertCommas (a:[])   = [a]
    insertCommas (a:b:[]) = [a,b]
    insertCommas (a:b:c:[]) = [a,b,c]
    insertCommas (a:b:c:rest) = a : b : c : ',' : insertCommas rest

-- | Format ecliptic position with DMS and comma-separated km
formatPosition :: String -> EclipticCoord -> String
formatPosition name (EclipticCoord (Degrees lon) (Degrees lat) dist) =
  name ++ ": lon " ++ showDMS lon ++ "  lat " ++ showDMS lat
       ++ "  dist " ++ showComma (round dist :: Int) ++ " km"
