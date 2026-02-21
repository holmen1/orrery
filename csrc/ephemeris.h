#ifndef EPHEMERIS_H
#define EPHEMERIS_H

/* Geocentric ecliptic coordinates */
typedef struct {
    double longitude;  /* degrees */
    double latitude;   /* degrees */
    double distance;   /* km */
} EclipticCoord;

/* Compute Moon position for a given Julian Day */
EclipticCoord moon_position(double jd);

/* Compute Sun position for a given Julian Day */
EclipticCoord sun_position(double jd);

#endif /* EPHEMERIS_H */
