/*
gcc rect-collision.c -o rect-collision -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./rect-collision
*/
#include "raylib.h"
#include "raymath.h"

#include <stdlib.h> // Required for: copysignf()

//void checkCollisions(Rectangle *boxA,Rectangle *boxB);

    Vector2 centerA, centerB;
    Vector2 subtract;
    Vector2 halfWidthA, halfWidthB;
    float minDistX, minDistY;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area");

    // Moving box
    Rectangle boxA = { 10, GetScreenHeight()/2.0f - 50, 200, 100 };
    int boxASpeedX = 4;

    // Mouse moved box
    Rectangle boxB = { GetScreenWidth()/2.0f - 30, GetScreenHeight()/2.0f - 30, 60, 60 };

    int screenUpperLimit = 40;      // Top menu limits

    bool pause = false;             // Movement pause
    //


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        // Move box if not paused
        if (!pause) boxA.x += boxASpeedX;

        // Bounce box on x screen limits
        if (((boxA.x + boxA.width) >= GetScreenWidth()) || (boxA.x <= 0)) boxASpeedX *= -1;

        // Update player-controlled-box (box02)
        boxB.x = GetMouseX() - boxB.width/2;
        boxB.y = GetMouseY() - boxB.height/2;

        // Make sure Box B does not go out of move area limits
        if ((boxB.x + boxB.width) >= GetScreenWidth()) boxB.x = GetScreenWidth() - boxB.width;
        else if (boxB.x <= 0) boxB.x = 0;

        if ((boxB.y + boxB.height) >= GetScreenHeight()) boxB.y = GetScreenHeight() - boxB.height;
        else if (boxB.y <= screenUpperLimit) boxB.y = (float)screenUpperLimit;


        if (CheckCollisionRecs(boxA, boxB)){
            centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
            centerB = (Vector2){boxB.x + boxB.width / 2,boxB.y + boxB.height / 2}; 
            subtract = Vector2Subtract(centerA,centerB);
            halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
            halfWidthB  = (Vector2){ boxB.width*.5f, boxB.height*.5f };
            minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
            minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);
            
            /*
            if (minDistX < minDistY) {
                //collide left or right
                boxB.x -= copysignf(minDistX, subtract.x); 
            } else {
                //collide top or bottom
                boxB.y -= copysignf(minDistY, subtract.y); 
            } 
            */
        }

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawRectangleRec(boxA, GOLD);
            DrawRectangleRec(boxB, BLUE);

            DrawText(TextFormat("CenterA : %.2f %.2f",centerA.x,centerA.y),200,50,30,RED);
            DrawText(TextFormat("CenterB : %.2f %.2f",centerB.x,centerB.y),200,80,30,RED);
            DrawText(TextFormat("Subtract : %.2f %.2f",subtract.x,subtract.y),200,110,30,RED);
            DrawText(TextFormat("minDist : %.2f %.2f",minDistX,minDistY),200,140,30,RED);

            // Draw help instructions
            DrawText("Press SPACE to PAUSE/RESUME", 20, screenHeight - 35, 20, LIGHTGRAY);

            DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}

//void checkCollisions(Rectangle *boxA,Rectangle *boxB){
//}
