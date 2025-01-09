#include "world.h"

EnvItem world_r[MAX_R] = {
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){50, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){250, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){450, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){650, 200, 100, 150}
    },
};

EnvItem world_c[MAX_C] = {
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){50, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){150, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){250, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){350, 100}
    },
};

EnvItem world_t[MAX_T] = {
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{0, 0}, {50, 50}, {100, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = GREEN,
        .t_points = {{100, 0}, {150, 50}, {200, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{200, 0}, {250, 50}, {300, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{300, 0}, {350, 50}, {400, 0}}
    }
};

void DrawWorld(void)
{
    for (int i = 0; i < MAX_R; i++) {
        DrawRectangle(world_r[i].rect.x, world_r[i].rect.y, world_r[i].rect.width, world_r[i].rect.height, world_r[i].color);
    }
    for (int i = 0; i < MAX_C; i++) {
        DrawCircleV(world_c[i].position, world_c[i].radius, world_c[i].color);
    }
    for (int i = 0; i < MAX_T; i++) {
        DrawTriangle(world_t[i].t_points[0], world_t[i].t_points[1], world_t[i].t_points[2], world_t[i].color);
    }
}

