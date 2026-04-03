#pragma once

#include <stdbool.h>

typedef enum sign_t: bool { NEGATIVE, POSITIVE } sign_t;

#define sign(a)     ((a) > 0 ? POSITIVE : NEGATIVE)
#define abs(a)      ((a) >= 0 ? (a) : -(a))
#define mod(a, b)   ((((a) % (b)) + (b)) % (b))

typedef struct vec4_t {
    union {
        struct { float x, y, z, w; };
        float pos[4];
    };
} vec4_t;

typedef struct vec3_t {
    union {
        struct { float x, y, z; };
        float pos[3];
    };
} vec3_t;

typedef struct vec2_t {
    union { 
        struct { float x, y; };
        float pos[2];
    };
} vec2_t;

#define E       2.7182818284590452354f
#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647692f
#define HALF_PI 1.57079632679489661923f

#define vec2(px, py)            (vec2_t){ .x = px, .y = py }
#define vec3(px, py, pz)        (vec3_t){ .x = px, .y = py, .z = pz }
#define vec4(px, py, pz, pw)    (vec4_t){ .x = px, .y = py, .z = pz, .w = pw }

float sinfaprx(float x);
float cosfaprx(float x);

vec3_t vec3_rot_xz(vec3_t p, float angle);
vec3_t vec3_rot_xy(vec3_t p, float angle);
vec2_t vec3_project_to_vec2(vec3_t p);
vec3_t vec3_translate_z(vec3_t p, float dz);

static inline float wrap_pi(float x)
{
    while (x < -PI) x += TWO_PI;
    while (x >  PI) x -= TWO_PI;
    return x;
}

#ifdef SMLIB_IMPLEMENTATION
float sinfaprx(float x)
{
    x = wrap_pi(x);
    float x2 = x * x;
    float x3 = x * x2;
    float x5 = x3 * x2;
    float x7 = x5 * x2;
    return (x - x3 / 6.0f + x5 / 120.0f - x7 / 5040.0f);
}

float cosfaprx(float x)
{
    return sinfaprx(x + HALF_PI);   
}

vec3_t vec3_rot_xz(vec3_t p, float angle)
{
    const float c = cosfaprx(angle);
    const float s = sinfaprx(angle);
    return vec3(
                p.x * c - p.z * s,
                p.y,
                p.x * s + p.z * c
            );
}

vec3_t vec3_rot_xy(vec3_t p, float angle)
{
    const float c = cosfaprx(angle);
    const float s = sinfaprx(angle);
    return vec3(
            p.x * c - p.z * s, 
            p.x * s + p.y * c, 
            p.z
        );
}

vec2_t vec3_project_to_vec2(vec3_t p)
{
    return vec2(p.x / p.z, p.y / p.z);
}

vec3_t vec3_translate_z(vec3_t p, float dz)
{
    return vec3(p.x, p.y, p.z + dz);
}
#endif
