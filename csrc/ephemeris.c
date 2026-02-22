#include "ephemeris.h"
#include <math.h>

/*
 * Solar position — Meeus, Astronomical Algorithms, Ch. 25
 * "Low accuracy" method, ~0.01° precision.
 */

static const double DEG2RAD = 3.14159265358979323846 / 180.0;
static const double AU_KM   = 149597870.7;

void sun_position(double jd, double *out) {
    /* Julian centuries from J2000.0  (Eq. 25.1) */
    double T = (jd - 2451545.0) / 36525.0;

    /* Geometric mean longitude of the Sun  (Eq. 25.2) */
    double L0 = fmod(280.46646 + T * (36000.76983 + T * 0.0003032), 360.0);
    if (L0 < 0.0) L0 += 360.0;

    /* Mean anomaly of the Sun  (Eq. 25.3) */
    double M = fmod(357.52911 + T * (35999.05029 - T * 0.0001537), 360.0);
    if (M < 0.0) M += 360.0;
    double M_rad = M * DEG2RAD;

    /* Equation of the center  (p. 164) */
    double C = (1.914602 - T * (0.004817 + T * 0.000014)) * sin(M_rad)
             + (0.019993 - T * 0.000101)                   * sin(2.0 * M_rad)
             + 0.000289                                     * sin(3.0 * M_rad);

    /* Sun's true longitude and true anomaly */
    double sun_lon = L0 + C;
    double sun_v   = M  + C;

    /* Distance in AU  (Eq. 25.5) */
    double e = 0.016708634 - T * (0.000042037 + T * 0.0000001267);
    double R = 1.000001018 * (1.0 - e * e)
             / (1.0 + e * cos(sun_v * DEG2RAD));

    /* Apparent longitude: nutation + aberration  (p. 164) */
    double omega = 125.04 - 1934.136 * T;
    double apparent_lon = sun_lon - 0.00569
                        - 0.00478 * sin(omega * DEG2RAD);

    out[0] = apparent_lon;
    out[1] = 0.0;           /* latitude always < 0.001° */
    out[2] = R * AU_KM;
}

/*
 * Lunar position — Meeus, Astronomical Algorithms, Ch. 47
 *
 * Uses the major periodic terms from Table 47.A (longitude & distance)
 * and Table 47.B (latitude).  Precision ~0.07° in longitude, ~0.04° in
 * latitude — plenty for visual phase rendering.
 */
void moon_position(double jd, double *out) {
    double T  = (jd - 2451545.0) / 36525.0;
    double T2 = T * T;
    double T3 = T2 * T;
    double T4 = T3 * T;

    /* Fundamental arguments (degrees), Meeus Table 47.a */

    /* Moon's mean longitude, referred to the mean equinox of date */
    double Lp = fmod(218.3164477 + 481267.88123421 * T
                     - 0.0015786 * T2 + T3 / 538841.0
                     - T4 / 65194000.0, 360.0);
    if (Lp < 0.0) Lp += 360.0;

    /* Moon's mean elongation */
    double D = fmod(297.8501921 + 445267.1114034 * T
                    - 0.0018819 * T2 + T3 / 545868.0
                    - T4 / 113065000.0, 360.0);
    if (D < 0.0) D += 360.0;

    /* Sun's mean anomaly */
    double M = fmod(357.5291092 + 35999.0502909 * T
                    - 0.0001536 * T2 + T3 / 24490000.0, 360.0);
    if (M < 0.0) M += 360.0;

    /* Moon's mean anomaly */
    double Mp = fmod(134.9633964 + 477198.8675055 * T
                     + 0.0087414 * T2 + T3 / 69699.0
                     - T4 / 14712000.0, 360.0);
    if (Mp < 0.0) Mp += 360.0;

    /* Moon's argument of latitude (mean distance from ascending node) */
    double F = fmod(93.2720950 + 483202.0175233 * T
                    - 0.0036539 * T2 - T3 / 3526000.0
                    + T4 / 863310000.0, 360.0);
    if (F < 0.0) F += 360.0;

    /* Three further corrections (A1, A2, A3) */
    double A1 = fmod(119.75 + 131.849 * T, 360.0);
    double A2 = fmod( 53.09 + 479264.290 * T, 360.0);
    double A3 = fmod(313.45 + 481266.484 * T, 360.0);

    /* Eccentricity correction factor for terms involving M */
    double E  = 1.0 - 0.002516 * T - 0.0000074 * T2;
    double E2 = E * E;

    /* Convert to radians */
    double Dr  = D  * DEG2RAD;
    double Mr  = M  * DEG2RAD;
    double Mpr = Mp * DEG2RAD;
    double Fr  = F  * DEG2RAD;
    double A1r = A1 * DEG2RAD;
    double A2r = A2 * DEG2RAD;
    double A3r = A3 * DEG2RAD;

    /* ---------------------------------------------------------------
     * Table 47.A — Periodic terms for the Moon's longitude (Σl)
     *              and distance (Σr)
     *
     *  Each row: D, M, Mp, F, coeff_l, coeff_r
     *  Σl in units of 0.000001°,  Σr in units of 0.001 km
     * --------------------------------------------------------------- */
    static const int tblLR[][6] = {
        { 0,  0,  1,  0,  6288774, -20905355 },
        { 2,  0, -1,  0,  1274027,  -3699111 },
        { 2,  0,  0,  0,   658314,  -2955968 },
        { 0,  0,  2,  0,   213618,   -569925 },
        { 0,  1,  0,  0,  -185116,     48888 },
        { 0,  0,  0,  2,  -114332,     -3149 },
        { 2,  0, -2,  0,    58793,    246158 },
        { 2, -1, -1,  0,    57066,   -152138 },
        { 2,  0,  1,  0,    53322,   -170733 },
        { 2, -1,  0,  0,    45758,   -204586 },
        { 0,  1, -1,  0,   -40923,   -129620 },
        { 1,  0,  0,  0,   -34720,    108743 },
        { 0,  1,  1,  0,   -30383,    104755 },
        { 2,  0,  0, -2,    15327,     10321 },
        { 0,  0,  1,  2,   -12528,         0 },
        { 0,  0,  1, -2,    10980,     79661 },
        { 4,  0, -1,  0,    10675,    -34782 },
        { 0,  0,  3,  0,    10034,    -23210 },
        { 4,  0, -2,  0,     8548,    -21636 },
        { 2,  1, -1,  0,    -7888,     24208 },
        { 2,  1,  0,  0,    -6766,     30824 },
        { 1,  0, -1,  0,    -5163,     -8379 },
        { 1,  1,  0,  0,     4987,    -16675 },
        { 2, -1,  1,  0,     4036,    -12831 },
        { 2,  0,  2,  0,     3994,    -10445 },
        { 4,  0,  0,  0,     3861,    -11650 },
        { 2,  0, -3,  0,     3665,     14403 },
        { 0,  1, -2,  0,    -2689,     -7003 },
        { 2,  0, -1,  2,    -2602,         0 },
        { 2, -1, -2,  0,     2390,     10056 },
        { 1,  0,  1,  0,    -2348,      6322 },
        { 2, -2,  0,  0,     2236,     -9884 },
        { 0,  1,  2,  0,    -2120,      5751 },
        { 0,  2,  0,  0,    -2069,         0 },
        { 2, -2, -1,  0,     2048,     -4950 },
        { 2,  0,  1, -2,    -1773,      4130 },
        { 2,  0,  0,  2,    -1595,         0 },
        { 4, -1, -1,  0,     1215,     -3958 },
        { 0,  0,  2,  2,    -1110,         0 },
        { 3,  0, -1,  0,     -892,      3258 },
        { 2,  1,  1,  0,     -810,      2616 },
        { 4, -1, -2,  0,      759,     -1897 },
        { 0,  2, -1,  0,     -713,     -2117 },
        { 2,  2, -1,  0,     -700,      2354 },
        { 2,  1, -2,  0,      691,         0 },
        { 2, -1,  0, -2,      596,         0 },
        { 4,  0,  1,  0,      549,     -1423 },
        { 0,  0,  4,  0,      537,     -1117 },
        { 4, -1,  0,  0,      520,     -1571 },
        { 1,  0, -2,  0,     -487,     -1739 },
        { 2,  1,  0, -2,     -399,         0 },
        { 0,  0,  2, -2,     -381,     -4421 },
        { 1,  1,  1,  0,      351,         0 },
        { 3,  0, -2,  0,     -340,         0 },
        { 4,  0, -3,  0,      330,         0 },
        { 2, -1,  2,  0,      327,         0 },
        { 0,  2,  1,  0,     -323,      1165 },
        { 1,  1, -1,  0,      299,         0 },
        { 2,  0,  3,  0,      294,         0 },
        { 2,  0, -1, -2,        0,      8752 },
    };
    int nLR = sizeof(tblLR) / sizeof(tblLR[0]);

    /* ---------------------------------------------------------------
     * Table 47.B — Periodic terms for the Moon's latitude (Σb)
     *
     *  Each row: D, M, Mp, F, coeff_b
     *  Σb in units of 0.000001°
     * --------------------------------------------------------------- */
    static const int tblB[][5] = {
        { 0,  0,  0,  1,  5128122 },
        { 0,  0,  1,  1,   280602 },
        { 0,  0,  1, -1,   277693 },
        { 2,  0,  0, -1,   173237 },
        { 2,  0, -1,  1,    55413 },
        { 2,  0, -1, -1,    46271 },
        { 2,  0,  0,  1,    32573 },
        { 0,  0,  2,  1,    17198 },
        { 2,  0,  1, -1,     9266 },
        { 0,  0,  2, -1,     8822 },
        { 2, -1,  0, -1,     8216 },
        { 2,  0, -2, -1,     4324 },
        { 2,  0,  1,  1,     4200 },
        { 2,  1,  0, -1,    -3359 },
        { 2, -1, -1,  1,     2463 },
        { 2, -1,  0,  1,     2211 },
        { 2, -1, -1, -1,     2065 },
        { 0,  1, -1, -1,    -1870 },
        { 4,  0, -1, -1,     1828 },
        { 0,  1,  0,  1,    -1794 },
        { 0,  0,  0,  3,    -1749 },
        { 0,  1, -1,  1,    -1565 },
        { 1,  0,  0,  1,    -1491 },
        { 0,  1,  1,  1,    -1475 },
        { 0,  1,  1, -1,    -1410 },
        { 0,  1,  0, -1,    -1344 },
        { 1,  0,  0, -1,    -1335 },
        { 0,  0,  3,  1,     1107 },
        { 4,  0,  0, -1,     1021 },
        { 4,  0, -1,  1,      833 },
        { 0,  0,  1, -3,      777 },
        { 4,  0, -2,  1,      671 },
        { 2,  0,  0, -3,      607 },
        { 2,  0,  2, -1,      596 },
        { 2, -1,  1, -1,      491 },
        { 2,  0, -2,  1,     -451 },
        { 0,  0,  3, -1,      439 },
        { 2,  0,  2,  1,      422 },
        { 2,  0, -3, -1,      421 },
        { 2,  1, -1,  1,     -366 },
        { 2,  1,  0,  1,     -351 },
        { 4,  0,  0,  1,      331 },
        { 2, -1,  1,  1,      315 },
        { 2, -2,  0, -1,      302 },
        { 0,  0,  1,  3,     -283 },
        { 2,  1,  1, -1,     -229 },
        { 1,  1,  0, -1,      223 },
        { 1,  1,  0,  1,      223 },
        { 0,  1, -2, -1,     -220 },
        { 2,  1, -1, -1,     -220 },
        { 1,  0,  1,  1,     -185 },
        { 2, -1, -2, -1,      181 },
        { 0,  1,  2,  1,     -177 },
        { 4,  0, -2, -1,      176 },
        { 4, -1, -1, -1,      166 },
        { 1,  0,  1, -1,     -164 },
        { 4,  0,  1, -1,      132 },
        { 1,  0, -1, -1,     -119 },
        { 4, -1,  0, -1,      115 },
        { 2, -2,  0,  1,      107 },
    };
    int nB = sizeof(tblB) / sizeof(tblB[0]);

    /* Accumulate sums */
    double Sl = 0.0;   /* Σl  (units: 0.000001°) */
    double Sr = 0.0;   /* Σr  (units: 0.001 km)  */
    double Sb = 0.0;   /* Σb  (units: 0.000001°) */

    for (int i = 0; i < nLR; i++) {
        double arg = tblLR[i][0] * Dr + tblLR[i][1] * Mr
                   + tblLR[i][2] * Mpr + tblLR[i][3] * Fr;
        double eCorr = 1.0;
        int mAbs = tblLR[i][1] < 0 ? -tblLR[i][1] : tblLR[i][1];
        if (mAbs == 1) eCorr = E;
        if (mAbs == 2) eCorr = E2;
        Sl += tblLR[i][4] * eCorr * sin(arg);
        Sr += tblLR[i][5] * eCorr * cos(arg);
    }

    for (int i = 0; i < nB; i++) {
        double arg = tblB[i][0] * Dr + tblB[i][1] * Mr
                   + tblB[i][2] * Mpr + tblB[i][3] * Fr;
        double eCorr = 1.0;
        int mAbs = tblB[i][1] < 0 ? -tblB[i][1] : tblB[i][1];
        if (mAbs == 1) eCorr = E;
        if (mAbs == 2) eCorr = E2;
        Sb += tblB[i][4] * eCorr * sin(arg);
    }

    /* Additional corrections (Meeus p. 338) */
    Sl +=  3958.0 * sin(A1r)
        +  1962.0 * sin(Lp * DEG2RAD - Fr)
        +   318.0 * sin(A2r);

    Sb += -2235.0 * sin(Lp * DEG2RAD)
        +   382.0 * sin(A3r)
        +   175.0 * sin(A1r - Fr)
        +   175.0 * sin(A1r + Fr)
        +   127.0 * sin(Lp * DEG2RAD - Mpr)
        -   115.0 * sin(Lp * DEG2RAD + Mpr);

    /* Final coordinates */
    double lon  = Lp + Sl / 1000000.0;
    double lat  = Sb / 1000000.0;
    double dist = 385000.56 + Sr / 1000.0;   /* km */

    /* Normalize longitude to [0, 360) */
    lon = fmod(lon, 360.0);
    if (lon < 0.0) lon += 360.0;

    out[0] = lon;
    out[1] = lat;
    out[2] = dist;
}
