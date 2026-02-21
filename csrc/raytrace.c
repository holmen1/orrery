#include "raytrace.h"
#include <stdio.h>
#include <math.h>

/*
 * Raytracer — render a Moon sphere with Sun illumination.
 * TODO: Ray-sphere intersection, dot(normal, sunDir) shading, PPM output
 */
int render_moon(
    double sun_lon,  double sun_lat,  double sun_dist,
    double moon_lon, double moon_lat, double moon_dist,
    int width, int height,
    const char *output_path)
{
    (void)sun_lon; (void)sun_lat; (void)sun_dist;
    (void)moon_lon; (void)moon_lat; (void)moon_dist;

    FILE *fp = fopen(output_path, "wb");
    if (!fp) return -1;

    /* Placeholder: black image */
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; i++) {
        fputc(0, fp); fputc(0, fp); fputc(0, fp);
    }

    fclose(fp);
    return 0;
}
