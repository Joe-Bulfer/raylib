/*
gcc main.c world.c player-projectile.c -o main -I . -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./main
*/
//#include "raylib.h"
#include "world.h"
#include "player-projectile.h"
#define PL_RADIUS 30 //radius

int main()
{
    SetTargetFPS(30);
    const int screenWidth = 800;
    const int screenHeight = 500;

    Player player = { 0 };
    player.pos = (Vector2){ 400, 280 };
    player.color = RAYWHITE;
    Camera2D camera = { 0 };
    camera.target = player.pos;
    camera.zoom = 2.0f;//starting zoom position
    InitWindow(screenWidth, screenHeight, "idk yet");

    while (!WindowShouldClose())
    {
        UpdateCameraCenter(&camera, &player, screenWidth, screenHeight);
        CameraZoom(&camera, 1.5, 3);//min max scroll
        
        PlayerControls(&player, world_r);
        //player.pos.y += 3;
        //if (IsKeyPressed(KEY_SPACE)) player.pos.y + 13;
        BeginDrawing();
        ClearBackground(RED);
        BeginMode2D(camera);
        DrawCircleV(player.pos,PL_RADIUS,player.color);
        DrawWorld();
        EndMode2D();
        //debug
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
