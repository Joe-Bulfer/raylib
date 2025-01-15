/*
gcc blocks-SaveMap.c blocks.c player-projectile.c -o blocks-SaveMap -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./blocks-SaveMap
*/

#include "stdio.h" //for SaveMap
#include "blocks.h"
#include "player-projectile.h"
//#define CURSOR_COLOR (Color){155, 155, 155, 128} //trasparent grey
#define ERASER_PINK (Color){255, 105, 180, 128}

//texture and blockID should't be attached to each individual block, only the array, an array of rectangles is enough
//until time to mine, crafting properties, blocking/non-blocking etc. are created, this Block struct is not needed
typedef struct Block{
    Rectangle rect;
    //Texture2D text;
    //int blocking; water and background are non blocking
    //int integrity; //mining and explosions reduce integrity,increasing dark opaque
    //int blockID; //if player in water, slow down
}Block;
//background will not be made of blocks, but one big texture, only inside of structures will be non-blocking blocks

//it's only a stack when in inventory....
struct stack {
    int id;
    Rectangle blocks[64];
    int blocking;
    Texture2D tex;
    //int integrity;//paired with rectangle per block
};

//basically ids
enum {
    grass,
    dirt,
    stone
};


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
        //mousePos is floored in TransparentCursor
        //makes sense cause a cursor could be a precondition for placing or destroying blocks, or interacting with the world grid
        mousePos = GetScreenToWorld2D(GetMousePosition(), camera);

        //DrawWorld(however many arrays...)
        for (int i = 0; i < grassBlockCount; i++) {
            DrawTexture(grassTex, grassBlocks[i].x, grassBlocks[i].y, WHITE);
        }
        for (int i = 0; i < dirtBlockCount; i++) {
            DrawTexture(dirtTex, dirtBlocks[i].x, dirtBlocks[i].y, WHITE);
        }
        for (int i = 0; i < waterBlockCount; i++) {
            DrawTexture(waterTex, waterBlocks[i].x, waterBlocks[i].y, WHITE);
        }

        switch (activeSlot){
            case 0:
                TransparentCursorTex(&camera,grassTex);
                PlaceBlock(grassBlocks,&grassBlockCount,99,&grassTex,mousePos
                            //make sure other types of blocks aren't under cursor,prevent overlapping blocks
                           //dirtBlocks,&dirtBlockCount,waterBlocks,&waterBlockCount
                           );
                break;
            case 1:
                TransparentCursorTex(&camera,dirtTex);
                PlaceBlock(dirtBlocks,&dirtBlockCount,99,&dirtTex,mousePos);
                break;
            case 2:
                TransparentCursorTex(&camera,waterTex);
                PlaceBlock(waterBlocks,&waterBlockCount,99,&waterTex,mousePos);
                break;
            case 3:
                TransparentCursorColor(&camera,ERASER_PINK);
                RemoveBlock_3(grassBlocks,&grassBlockCount,
                            waterBlocks,&waterBlockCount,
                            dirtBlocks,&dirtBlockCount,
                            mousePos);
                
                break;
        }

        EndMode2D();
        //Debug(&camera, ..not sure how to put into function...);
        DrawText(TextFormat("mousePos %.1f %.1f",mousePos.x,mousePos.y),70,50,20,BLACK);
        DrawText(TextFormat("water %d/%d",grassBlockCount,99),70,80,20,BLACK);
        DrawText(TextFormat("dirt %d/%d",dirtBlockCount,99),70,110,20,BLACK);
        DrawText(TextFormat("water %d/%d",waterBlockCount,99),70,140,20,BLACK);

        EndDrawing();
    }

    //SaveMap(grassBlocks,dirtBlocks,waterBlocks); //------------------
    // Write the block's x y position and ID to file
    FILE *file = fopen("blocks-main_SaveMap.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for saving.\n");
        return 1;
    }
    for (int i = 0; i < grassBlockCount; i++) {
        fprintf(file, "%.0f,%.0f,%d\n", grassBlocks[i].x, grassBlocks[i].y, 0);
    }
    for (int i = 0; i < dirtBlockCount; i++) {
        fprintf(file, "%.0f,%.0f,%d\n", dirtBlocks[i].x, dirtBlocks[i].y, 1);
    }
    for (int i = 0; i < waterBlockCount; i++) {
        fprintf(file, "%.0f,%.0f,%d\n", waterBlocks[i].x, waterBlocks[i].y, 2);
    }

    fclose(file);

    UnloadTexture(grassTex);
    UnloadTexture(dirtTex);
    UnloadTexture(waterTex);
    
    CloseWindow();
}
