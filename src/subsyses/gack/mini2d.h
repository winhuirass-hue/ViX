#ifndef GACK_H
#define GACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* =========================
 * Basic Types
 * ========================= */

typedef struct {
    float r, g, b, a;
} m2d_color_t;

typedef struct {
    int x;
    int y;
} m2d_point_t;

typedef struct {
    int width;
    int height;
    int stride;      /* bytes per row */
    void* pixels;    /* RGBA8888 */
} m2d_surface_t;

/* =========================
 * Context
 * ========================= */

typedef struct m2d_context m2d_context_t;

/* =========================
 * API
 * ========================= */

m2d_context_t* m2d_create(m2d_surface_t* surface);
void           m2d_destroy(m2d_context_t* ctx);

/* State */
void m2d_set_color(m2d_context_t* ctx, m2d_color_t color);

/* Drawing */
void m2d_clear   (m2d_context_t* ctx);
void m2d_pixel   (m2d_context_t* ctx, int x, int y);
void m2d_line    (m2d_context_t* ctx, int x0, int y0, int x1, int y1);
void m2d_rect    (m2d_context_t* ctx, int x, int y, int w, int h);
void m2d_fillrect(m2d_context_t* ctx, int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif

#endif /* GACK_H */
