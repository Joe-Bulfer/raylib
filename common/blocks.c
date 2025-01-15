/*
gcc blocks.c -o blocks -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./blocks
*/
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <math.h>

#define TRANSPARENT_WHITE (Color){255, 255, 255, 128}
#define GREY (Color){55, 55, 55, 128}
//Vector2 mousePos = { 0.0f, 0.0f };
Vector2 mousePos = { 0 };

static Vector2 previousMousePos = {0};
static bool isDragging = false;
void CameraDrag(Camera2D *camera) {
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // If the left mouse button is pressed, start dragging
        previousMousePos = mousePos;
        isDragging = true;
    }
    if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        // If the left mouse button is held down and we're in drag mode
        Vector2 delta = { mousePos.x - previousMousePos.x, mousePos.y - previousMousePos.y };
        
        // Update the camera's target position based on the mouse movement
        camera->target.x -= delta.x; 
        camera->target.y -= delta.y;

        // Update the previous mouse position
        previousMousePos = mousePos;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        // When the left mouse button is released, stop dragging
        isDragging = false;
    }
}
void TransparentCursorTex(Camera2D *camera,Texture2D tex){  
    mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    mousePos.x = floorf(mousePos.x / 10) * 10.0f;
    mousePos.y = floorf(mousePos.y / 10) * 10.0f;
    DrawTexture(tex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
}
void TransparentCursorColor(Camera2D *camera,Color color){  
    mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    mousePos.x = floorf(mousePos.x / 10) * 10.0f;
    mousePos.y = floorf(mousePos.y / 10) * 10.0f;
    //DrawTexture(sandTex, mousePos.x, mousePos.y, GREY);
    DrawRectangle(mousePos.x,mousePos.y,10,10,color);
}
//TransparentCursorColor(Camera2D *camera,Color color){  
//blockCount is number currently placed,blockLen is MAX_R for world_R array, etc.
int areaFree = 1;

void PlaceBlock(Rectangle *blocks,int *blockCount,int blockLen,Texture2D *tex,Vector2 mousePos){
    // Check if the spot is already occupied
    // will have to check additional once there are multiple stacks of blocks
    areaFree = 1;
    for (int i = 0; i < *blockCount; i++) {
        Vector2 envPos = (Vector2){ blocks[i].x, blocks[i].y };
        if (Vector2Equals(envPos, mousePos)) {
            areaFree  = 0;
            break; }
    }
    if (IsMouseButtonDown(0) && areaFree && (*blockCount < blockLen)){
    //if (IsMouseButtonDown(0)){
        blocks[*blockCount] = (Rectangle){mousePos.x,mousePos.y,10,10};
        (*blockCount)++;
    }
}

void UpdateActiveSlot(int *activeSlot){
    if (IsKeyPressed(KEY_ONE)) {
        *activeSlot = 0;
        //return 0;
    } else if (IsKeyPressed(KEY_TWO)) {
        *activeSlot = 1;
        //return 1;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        *activeSlot = 2;
        //return 2;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        *activeSlot = 3;
        //return 2;
    }
    //else return activeSlot
}
void RemoveBlock_3(Rectangle *b1, int *bc1,
                   Rectangle *b2, int *bc2,
                   Rectangle *b3, int *bc3,
                   Vector2 mousePos){
    //int totalLen = bc1+bc2+bc3;//combine all blocCounts
    if (IsMouseButtonDown(0)){

        for (int i = 0; i < *bc1; i++) {
            if (CheckCollisionPointRec(mousePos, b1[i])) {
                // Shift blocks down in the array
                for (int j = i; j < *bc1 - 1; j++) {
                    b1[j] = b1[j + 1];
                }
                //blocks[i] = blocks[*blockCount + 1];
                (*bc1)--;
                //break;
                return;
            }
        }
        for (int i = 0; i < *bc2; i++) {
            if (CheckCollisionPointRec(mousePos, b2[i])) {
                // Shift blocks down in the array
                for (int j = i; j < *bc2 - 1; j++) {
                    b2[j] = b2[j + 1];
                }
                //blocks[i] = blocks[*blockCount + 1];
                (*bc2)--;
                //break;
                return;
                //
            }
        }
        for (int i = 0; i < *bc3; i++) {
            if (CheckCollisionPointRec(mousePos, b3[i])) {
                // Shift blocks down in the array
                for (int j = i; j < *bc3 - 1; j++) {
                    b3[j] = b3[j + 1];
                }
                //blocks[i] = blocks[*blockCount + 1];
                (*bc3)--;
                //break;
                return;
                //
            }
        }

    }
}
void RemoveBlock(Rectangle *blocks, int *blockCount, Vector2 mousePos){
    //lazy delete - stuff last element into found index. We could shift all of em down but we don't care about order 
    //arr[i] = arr[arrLen-1]

    if (IsMouseButtonDown(0)){
        for (int i = 0; i < *blockCount; i++) {
            if (CheckCollisionPointRec(mousePos, blocks[i])) {
                // Shift blocks down in the array
                for (int j = i; j < *blockCount - 1; j++) {
                    blocks[j] = blocks[j + 1];
                }
                //blocks[i] = blocks[*blockCount + 1];
                (*blockCount)--;
                break;
            }
        }
    }
}

