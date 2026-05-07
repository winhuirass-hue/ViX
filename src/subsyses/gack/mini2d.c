#include "mini2d.h"
#include <stdlib.h>
#include <string.h>

/* =========================
 * Internal Context
 * ========================= */

struct m2d_context {
    m2d_surface_t* surface;
    m2d_color_t    color;
};

/* =========================
 * Helpers
 * ========================= */

static inline uint32_t pack_rgba(m2d_color_t c) {
    if (c.r < 0.0f) c.r = 0.0f; if (c.r > 1.0f) c.r = 1.0f;
    if (c.g < 0.0f) c.g = 0.0f; if (c.g > 1.0f) c.g = 1.0f;
    if (c.b < 0.0f) c.b = 0.0f; if (c.b > 1.0f) c.b = 1.0f;
    if (c.a < 0.0f) c.a = 0.0f; if (c.a > 1.0f) c.a = 1.0f;
    
    uint8_t r = (uint8_t)(c.r * 255.0f);
    uint8_t g = (uint8_t)(c.g * 255.0f);
    uint8_t b = (uint8_t)(c.b * 255.0f);
    uint8_t a = (uint8_t)(c.a * 255.0f);

    return (a << 24) | (r << 16) | (g << 8) | b;
}

/* =========================
 * Context
 * ========================= */

m2d_context_t* m2d_create(m2d_surface_t* surface) {
    if (!surface || !surface->pixels)
        return NULL;
    
    if (surface->width <= 0 || surface->height <= 0)
        return NULL;

    if (surface->stride < (int)sizeof(uint32_t) * surface->width)
        return NULL;

    m2d_context_t* ctx = (m2d_context_t*)malloc(sizeof(*ctx));
    if (!ctx)
        return NULL;

    ctx->surface = surface;
    ctx->color   = (m2d_color_t){1, 1, 1, 1};

    return ctx;
}

void m2d_destroy(m2d_context_t* ctx) {
    free(ctx);
}

void m2d_set_color(m2d_context_t* ctx, m2d_color_t color) {
    if (!ctx)
        return;

    ctx->color = color;
}

/* =========================
 * Drawing
 * ========================= */

void m2d_clear(m2d_context_t* ctx) {
    if (!ctx || !ctx->surface || !ctx->surface->pixels)
        return;
    
    m2d_surface_t* s = ctx->surface;
    uint32_t color = pack_rgba(ctx->color);

    for (int y = 0; y < s->height; y++) {
        uint32_t* row = (uint32_t*)((uint8_t*)s->pixels + y * s->stride);
        for (int x = 0; x < s->width; x++) {
            row[x] = color;
        }
    }
}

void m2d_pixel(m2d_context_t* ctx, int x, int y) {
    if (!ctx || !ctx->surface || !ctx->surface->pixels)
        return;
    
    m2d_surface_t* s = ctx->surface;
    if (x < 0 || y < 0 || x >= s->width || y >= s->height)
        return;

    uint32_t* row = (uint32_t*)((uint8_t*)s->pixels + y * s->stride);
    row[x] = pack_rgba(ctx->color);
}

/* Bresenham */
void m2d_line(m2d_context_t* ctx, int x0, int y0, int x1, int y1) {
    if (!ctx || !ctx->surface || !ctx->surface->pixels)
        return;
    
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        m2d_pixel(ctx, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void m2d_rect(m2d_context_t* ctx, int x, int y, int w, int h) {
    if (!ctx || w <= 0 || h <= 0)
        return;

    m2d_line(ctx, x, y, x + w - 1, y);
    m2d_line(ctx, x, y, x, y + h - 1);
    m2d_line(ctx, x + w - 1, y, x + w - 1, y + h - 1);
    m2d_line(ctx, x, y + h - 1, x + w - 1, y + h - 1);
}

void m2d_fillrect(m2d_context_t* ctx, int x, int y, int w, int h) {
    if (!ctx || w <= 0 || h <= 0)
        return;

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            m2d_pixel(ctx, x + i, y + j);
        }
    }
}
