/*
gcc better-envItems.c -o better-envItems -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./better-envItems
*/
#include "raylib.h"
#include "world.h" // Include the world header

int main()
{
    int screenWidth = 800;
    int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "BoilerPlate");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RED);
        DrawWorld();  // Call the function to draw world items
        EndDrawing();
    }

    CloseWindow(); // Close the window

    return 0;
}

