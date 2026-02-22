module Orrery.Time
  ( JulianDay(..)
  , toJulianDay
  ) where

-- | Julian Day number
newtype JulianDay = JulianDay Double deriving (Show)

-- | Convert calendar date (UTC) to Julian Day number.
--   Meeus, Astronomical Algorithms, Ch. 7, Eq. 7.1
--   Valid for all Gregorian calendar dates (after 1582-10-15).
toJulianDay :: Int -> Int -> Int -> Int -> Int -> Int -> JulianDay
toJulianDay year month day hour minute second =
  let -- If Jan or Feb, treat as month 13/14 of previous year
      (y, m) = if month <= 2
               then (fromIntegral year - 1 :: Double, fromIntegral month + 12 :: Double)
               else (fromIntegral year     :: Double, fromIntegral month      :: Double)
      -- Day fraction from time
      dayFrac = fromIntegral hour / 24.0
              + fromIntegral minute / 1440.0
              + fromIntegral second / 86400.0
      d       = fromIntegral day + dayFrac
      -- Gregorian calendar correction
      a = floor (y / 100.0) :: Int
      b = 2 - a + div a 4
      -- Julian Day Number
      jd = fromIntegral (floor (365.25 * (y + 4716.0)) :: Int)
         + fromIntegral (floor (30.6001 * (m + 1.0))   :: Int)
         + d
         + fromIntegral b
         - 1524.5
  in  JulianDay jd
