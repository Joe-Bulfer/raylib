/*
gcc blocks-main.c blocks.c player-projectile.c -o blocks -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./blocks
*/
#include "blocks.h"
#include "player-projectile.h"
#define CURSOR_COLOR (Color){155, 155, 155, 128}

typedef struct Block{
    Rectangle rect;
    Texture2D text;
}Block;
//nv -O ../map-maker.c blocks-main.c 
//todo: get one texture block image and be able to place blocks
Block blocks[20];
int blockCount = 0;
int canPlace = 0;

int main(){
    int screenWidth, screenHeight;
    screenWidth = 800; screenHeight = 500;
    InitWindow(screenWidth,screenHeight,"BoilerPlate");
    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Camera target (center of screen)
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Camera offset (center of screen)
    camera.zoom = 1.0f; // Camera zoom (1.0 = no zoom)
    //camera.target = player.position;
    SetTargetFPS(30);
    while(!WindowShouldClose()){
        CameraZoom(&camera,.5,3);
        CameraDrag(&camera);
        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        TransparentCursor(&camera);
        DrawRectangle(mousePos.x,mousePos.y,10,10,CURSOR_COLOR);
        EndMode2D();
        //Debug(&mousePos,&camera);
        DrawText(TextFormat("mousePos %.1f %.1f",mousePos.x,mousePos.y),100,100,20,BLACK);
        EndDrawing();
    }
    CloseWindow();
}
