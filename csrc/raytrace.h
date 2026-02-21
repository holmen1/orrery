#ifndef RAYTRACE_H
#define RAYTRACE_H

/* Render the Moon as seen from Earth, illuminated by the Sun.
 * sun_lon/lat and moon_lon/lat in degrees, distances in km.
 * Writes a PPM image to the given file path.
 * width and height are image dimensions in pixels.
 */
int render_moon(
    double sun_lon,  double sun_lat,  double sun_dist,
    double moon_lon, double moon_lat, double moon_dist,
    int width, int height,
    const char *output_path
);

#endif /* RAYTRACE_H */
