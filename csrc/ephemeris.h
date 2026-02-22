#ifndef EPHEMERIS_H
#define EPHEMERIS_H

/*
 * Geocentric ecliptic position for a given Julian Day number.
 *
 * Results written into out[3]:
 *   out[0]  longitude  degrees  [0, 360)
 *   out[1]  latitude   degrees  (negative = south of ecliptic)
 *   out[2]  distance   km
 *
 * Algorithms: Jean Meeus, Astronomical Algorithms, 2nd ed.
 *   Sun  — Ch. 25 (low accuracy, ~0.01°)
 *   Moon — Ch. 47 (~60 periodic terms, ~0.07°)
 */
void sun_position(double jd, double *out);
void moon_position(double jd, double *out);

#endif /* EPHEMERIS_H */
