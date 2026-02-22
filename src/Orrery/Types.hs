module Orrery.Types
  ( Angle(..)
  , Degrees(..)
  , Radians(..)
  , EclipticCoord(..)
  , EquatorialCoord(..)
  , HorizonCoord(..)
  , Observer(..)
  ) where

-- | Angle wrapper for type safety
newtype Degrees = Degrees Double deriving (Show)
newtype Radians = Radians Double deriving (Show)

class Angle a where
  toDegrees :: a -> Degrees
  toRadians :: a -> Radians

instance Angle Degrees where
  toDegrees = id
  toRadians (Degrees d) = Radians (d * pi / 180.0)

instance Angle Radians where
  toDegrees (Radians r) = Degrees (r * 180.0 / pi)
  toRadians = id

-- | Geocentric ecliptic coordinates (from C ephemeris)
data EclipticCoord = EclipticCoord
  { eclLon  :: !Degrees  -- ^ Ecliptic longitude
  , eclLat  :: !Degrees  -- ^ Ecliptic latitude
  , eclDist :: !Double   -- ^ Distance in km
  } deriving (Show)

-- | Equatorial coordinates
data EquatorialCoord = EquatorialCoord
  { ra  :: !Degrees  -- ^ Right ascension
  , dec :: !Degrees  -- ^ Declination
  } deriving (Show)

-- | Horizontal coordinates (what you see in the sky)
data HorizonCoord = HorizonCoord
  { altitude :: !Degrees  -- ^ Above horizon
  , azimuth  :: !Degrees  -- ^ Compass bearing
  } deriving (Show)

-- | Observer on Earth's surface
data Observer = Observer
  { obsLat :: !Degrees  -- ^ Latitude  (N positive)
  , obsLon :: !Degrees  -- ^ Longitude (E positive)
  } deriving (Show)
