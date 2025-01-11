/*
gcc blocks.c -o blocks -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./blocks
*/
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <math.h>

#define TRANSPARENT_WHITE (Color){255, 255, 255, 128}
//Vector2 mousePos = { 0.0f, 0.0f };
Vector2 mousePos = { 0 };
int areaFree = 0;

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
void TransparentCursorTexture(Camera2D *camera,Texture2D text){  
    mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    mousePos.x = floorf(mousePos.x / 10) * 10.0f;
    mousePos.y = floorf(mousePos.y / 10) * 10.0f;
    DrawTexture(text, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
}
void TransparentCursor(Camera2D *camera){  
    mousePos = GetScreenToWorld2D(GetMousePosition(), *camera);
    mousePos.x = floorf(mousePos.x / 10) * 10.0f;
    mousePos.y = floorf(mousePos.y / 10) * 10.0f;
    //DrawTexture(sandTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
    DrawRectangle(mousePos.x,mousePos.y,10,10,TRANSPARENT_WHITE);
}
//TransparentCursorColor(Camera2D *camera,Color color){  
//blockCount is number currently placed,blockLen is MAX_R for world_R array, etc.
void PlaceBlock(EnvItem **blocks,int blockCount,int blockLen){
    // Check if the spot is already occupied
    // will have to check additional once there are multiple stacks of blocks
    for (int i = 0; i < blockCount; i++) {
        Vector2 envPos = (Vector2){ blocks[i]->rect.x, blocks[i]->rect.y };
        if (Vector2Equals(envPos, mousePos)) {
            areaFree  = 0;
            break; }
    }
    if (IsMouseButtonDown(0) && areaFree && (blockCount < blockLen)){
        int x = 1;
    }
}

