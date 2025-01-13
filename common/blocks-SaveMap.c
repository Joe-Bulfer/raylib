/*
gcc blocks-SaveMap.c blocks.c player-projectile.c -o blocks-SaveMap -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./blocks-SaveMap
*/

#include "stdio.h" //for SaveMap
#include "blocks.h"
#include "player-projectile.h"
#define CURSOR_COLOR (Color){155, 155, 155, 128}

//texture and blockID should't be attached to each individual block, only the array, an array of rectangles is enough
typedef struct Block{
    Rectangle rect;
    //Texture2D text;
    //int blocking; water and background are non blocking
}Block;
//background will not be made of blocks, but one big texture, only inside of structures will be non-blocking blocks

//x, y, and id is written to file in SaveMap
Rectangle grassBlocks[99];//id 0
int grassBlockCount = 0;
Rectangle  dirtBlocks[99];//id 1
int dirtBlockCount = 0;
Rectangle  waterBlocks[99];//id 2
int waterBlockCount = 0;

int canPlace = 0;//make sure cursor isn't over existing block before PlaceBlock

int activeSlot = 0;//determines which block to place

int main(){
    int screenWidth, screenHeight;
    screenWidth = 800; screenHeight = 500;
    InitWindow(screenWidth,screenHeight,"BoilerPlate");
    Texture2D grassTex = LoadTexture("../assets/grass-10x10.png");        
    Texture2D dirtTex = LoadTexture("../assets/dirt-10x10.png");        
    Texture2D waterTex = LoadTexture("../assets/water-10x10.png");        
    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Camera target (center of screen)
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Camera offset (center of screen)
    camera.zoom = 1.0f; // Camera zoom (1.0 = no zoom)
    //camera.target = player.position;
    SetTargetFPS(30);
    while(!WindowShouldClose()){
        UpdateActiveSlot(&activeSlot);//check for num key presses
        CameraZoom(&camera,.5,3);//min max
        CameraDrag(&camera); //right click, drag to move around
        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        //TransparentCursor(&camera);
        mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        switch (activeSlot){
            case 0:
                TransparentCursorTex(&camera,grassTex);
                PlaceBlock(grassBlocks,&grassBlockCount,99,&grassTex,mousePos);
                break;
            case 1:
                TransparentCursorTex(&camera,dirtTex);
                PlaceBlock(dirtBlocks,&dirtBlockCount,99,&dirtTex,mousePos);
                break;
            case 2:
                TransparentCursorTex(&camera,waterTex);
                PlaceBlock(waterBlocks,&waterBlockCount,99,&waterTex,mousePos);
                break;
        }

        for (int i = 0; i < grassBlockCount; i++) {
            DrawTexture(grassTex, grassBlocks[i].x, grassBlocks[i].y, WHITE);
        }
        for (int i = 0; i < dirtBlockCount; i++) {
            DrawTexture(dirtTex, dirtBlocks[i].x, dirtBlocks[i].y, WHITE);
        }
        for (int i = 0; i < waterBlockCount; i++) {
            DrawTexture(waterTex, waterBlocks[i].x, waterBlocks[i].y, WHITE);
        }
        //DrawRectangle(mousePos.x,mousePos.y,10,10,CURSOR_COLOR);
        EndMode2D();
        //Debug(&mousePos,&camera);
        DrawText(TextFormat("mousePos %.1f %.1f",mousePos.x,mousePos.y),100,100,20,BLACK);
        DrawText(TextFormat("grassBlockCount %d",grassBlockCount ),100,130,20,BLACK);

        EndDrawing();
    }

    //SaveMap(grassBlocks,dirtBlocks,waterBlocks); //------------------
    FILE *file = fopen("blocks-main_SaveMap.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for saving.\n");
        return 1;
    }
    for (int i = 0; i < grassBlockCount; i++) {
        // Write the block's position and texture index to the file
        fprintf(file, "%.0f,%.0f,%d\n", grassBlocks[i].x, grassBlocks[i].y, 0);
    }
    for (int i = 0; i < dirtBlockCount; i++) {
        // Write the block's position and texture index to the file
        fprintf(file, "%.0f,%.0f,%d\n", dirtBlocks[i].x, dirtBlocks[i].y, 1);
    }
    for (int i = 0; i < waterBlockCount; i++) {
        // Write the block's position and texture index to the file
        fprintf(file, "%.0f,%.0f,%d\n", waterBlocks[i].x, waterBlocks[i].y, 2);
    }

    fclose(file);
    //SaveMap(grassBlocks,dirtBlocks,waterBlocks); //------------------
    UnloadTexture(grassTex);
    UnloadTexture(dirtTex);
    UnloadTexture(waterTex);
    
    CloseWindow();
}
