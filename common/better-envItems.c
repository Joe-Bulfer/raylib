//basically same as main.c right now
/*
gcc better-envItems.c world.c -o . -o better-envItems -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./better-envItems
*/
#include "world.h"

int main(){
    SetTargetFPS(30);
    int screenWidth, screenHeight;
    screenWidth = 800; screenHeight = 500;
    InitWindow(screenWidth,screenHeight,"BoilerPlate");
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RED);

        DrawWorld();

        EndDrawing();
    }
    CloseWindow();
}

