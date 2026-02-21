module Moon.Time
  ( JulianDay(..)
  , toJulianDay
  ) where

-- | Julian Day number
newtype JulianDay = JulianDay Double deriving (Show)

-- | Convert calendar date to Julian Day
--   year month day hour minute second (UTC)
--   TODO: Implement Meeus Ch. 7
toJulianDay :: Int -> Int -> Int -> Int -> Int -> Int -> JulianDay
toJulianDay _year _month _day _hour _minute _second =
  JulianDay 0.0
