#include "raytrace.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*
 * Raytracer — render a Moon sphere illuminated by the Sun.
 *
 * Coordinate system: geocentric ecliptic, km.
 * Camera at Earth (origin) looking toward the Moon.
 * Lambertian shading: brightness = dot(surface_normal, sun_direction).
 */

static const double DEG2RAD      = 3.14159265358979323846 / 180.0;
static const double MOON_RADIUS  = 1737.4;   /* km */

/* ── 3-vector ─────────────────────────────────────────────────────────── */

typedef struct { double x, y, z; } Vec3;

static Vec3 vadd(Vec3 a, Vec3 b)        { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 vsub(Vec3 a, Vec3 b)        { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 vscale(Vec3 a, double s)    { return (Vec3){a.x*s,   a.y*s,   a.z*s  }; }
static double vdot(Vec3 a, Vec3 b)      { return a.x*b.x + a.y*b.y + a.z*b.z;       }
static Vec3 vcross(Vec3 a, Vec3 b)      {
    return (Vec3){ a.y*b.z - a.z*b.y,
                   a.z*b.x - a.x*b.z,
                   a.x*b.y - a.y*b.x };
}
static Vec3 vnorm(Vec3 a)               { return vscale(a, 1.0 / sqrt(vdot(a, a))); }

/* ── Ecliptic lon/lat → unit direction vector ─────────────────────────── */

static Vec3 ecl_to_vec(double lon_deg, double lat_deg) {
    double lon = lon_deg * DEG2RAD;
    double lat = lat_deg * DEG2RAD;
    return (Vec3){ cos(lat) * cos(lon),
                   cos(lat) * sin(lon),
                   sin(lat) };
}

/* ── Ray–sphere intersection ──────────────────────────────────────────── */
/*  Ray: P = origin + t*dir                                                */
/*  Returns nearest positive t, or -1 if no hit.                           */

static double hit_sphere(Vec3 origin, Vec3 dir, Vec3 center, double radius) {
    Vec3   oc = vsub(origin, center);
    double a  = vdot(dir, dir);
    double b  = 2.0 * vdot(oc, dir);
    double c  = vdot(oc, oc) - radius * radius;
    double D  = b*b - 4.0*a*c;
    if (D < 0.0) return -1.0;
    double t  = (-b - sqrt(D)) / (2.0 * a);
    if (t < 0.0) t = (-b + sqrt(D)) / (2.0 * a);
    return t < 0.0 ? -1.0 : t;
}

/* ── Main render function ─────────────────────────────────────────────── */

int render_moon(
    double sun_lon,  double sun_lat,  double sun_dist,
    double moon_lon, double moon_lat, double moon_dist,
    int width, int height,
    const char *output_path)
{
    /* Moon and Sun in 3D ecliptic space (km) */
    Vec3 moon_center = vscale(ecl_to_vec(moon_lon, moon_lat), moon_dist);
    Vec3 sun_pos     = vscale(ecl_to_vec(sun_lon,  sun_lat),  sun_dist);

    /* Unit vector from Moon toward Sun: sets the illumination direction */
    Vec3 sun_dir = vnorm(vsub(sun_pos, moon_center));

    /* Camera at Earth (origin) looking toward the Moon */
    Vec3 cam_pos     = {0.0, 0.0, 0.0};
    Vec3 cam_forward = vnorm(moon_center);

    /* Camera frame: up = ecliptic north pole projected onto image plane */
    Vec3 world_up = {0.0, 0.0, 1.0};
    Vec3 cam_right = vnorm(vcross(cam_forward, world_up));
    Vec3 cam_up    = vcross(cam_right, cam_forward);

    /* FOV: scale so the Moon disc fills ~75 % of the frame height */
    double angular_r = atan(MOON_RADIUS / moon_dist);
    double half_fov  = angular_r / 0.75;
    double aspect    = (double)width / (double)height;

    /* Open output */
    FILE *fp = fopen(output_path, "wb");
    if (!fp) return -1;

    unsigned char *row = malloc(3 * width);
    if (!row) { fclose(fp); return -1; }

    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    for (int j = 0; j < height; j++) {
        /* v: +1 at top, -1 at bottom */
        double v = (1.0 - 2.0 * (j + 0.5) / height) * tan(half_fov);

        for (int i = 0; i < width; i++) {
            double u = (2.0 * (i + 0.5) / width - 1.0) * aspect * tan(half_fov);

            Vec3 ray_dir = vnorm(vadd(vadd(cam_forward,
                                          vscale(cam_right, u)),
                                      vscale(cam_up, v)));

            double t = hit_sphere(cam_pos, ray_dir, moon_center, MOON_RADIUS);

            unsigned char r, g, b;
            if (t > 0.0) {
                Vec3 hit    = vadd(cam_pos, vscale(ray_dir, t));
                Vec3 normal = vnorm(vsub(hit, moon_center));

                /* Lambertian diffuse + faint ambient (earthshine) */
                double diffuse    = vdot(normal, sun_dir);
                if (diffuse < 0.0) diffuse = 0.0;
                double ambient    = 0.03;
                double brightness = ambient + (1.0 - ambient) * diffuse;

                /* Lunar gray (warm) */
                r = (unsigned char)(brightness * 210);
                g = (unsigned char)(brightness * 205);
                b = (unsigned char)(brightness * 195);
            } else {
                r = g = b = 0;   /* space */
            }

            row[i*3+0] = r;
            row[i*3+1] = g;
            row[i*3+2] = b;
        }

        fwrite(row, 3, width, fp);
    }

    free(row);
    fclose(fp);
    return 0;
}
