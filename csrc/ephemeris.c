#include "ephemeris.h"
#include <math.h>

/*
 * Solar position — Meeus Ch. 25
 * TODO: Implement periodic terms
 */
EclipticCoord sun_position(double jd) {
    (void)jd;
    EclipticCoord c = {0.0, 0.0, 0.0};
    return c;
}

/*
 * Lunar position — Meeus Ch. 47
 * TODO: Implement ~60 periodic terms (longitude, latitude, distance)
 */
EclipticCoord moon_position(double jd) {
    (void)jd;
    EclipticCoord c = {0.0, 0.0, 0.0};
    return c;
}
