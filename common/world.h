#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

//rects circles and triangles
#define MAX_R 4

#define MAX_C 4
#define MAX_T 4


typedef struct EnvItem {
    Color color;
    enum { CIRCLE, TRIANGLE, RECTANGLE, POLYGON } type;
    union {
        struct { // Circle
            int radius;
            Vector2 position;
        };
        struct { // Triangle
            Vector2 t_points[3];
        };
        // Rectangle from raylib.h
        Rectangle rect;
        struct { // Polygon 
            int num_points;
            Vector2 *points;
        };
    };
} EnvItem;

//arrays to store shapes of map/world

extern EnvItem world_r[MAX_R];
extern EnvItem world_c[MAX_C];
extern EnvItem world_t[MAX_T];

//could also define non-blocking background or world textures

void DrawWorld(void);

#endif // WORLD_H

