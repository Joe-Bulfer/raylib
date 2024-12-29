/*
gcc box-area.c -o box-area -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./box-area
*/
#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>     // Required for: abs()

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Joe Bulfer Collision Example");

    // Box A: Moving box
    Rectangle boxA = { 10, GetScreenHeight()/2.0f +30, 200, 100 };
    int boxASpeedX = 4;

    // Box B: Mouse moved box
    Rectangle boxB = { GetScreenWidth()/2.0f - 30, GetScreenHeight()/2.0f - 30, 60, 60 };

    Rectangle boxCollision = { 0 }; // Collision rectangle

    int screenUpperLimit = 40;      // Top menu limits

    bool pause = false;             // Movement pause
    bool collision = false;         // Collision detection

    Vector2 centerA, centerB;
    Vector2 subtract;
    Vector2 halfWidthA,halfWidthB;
    float minDistX, minDistY;  
    bool leftOrRightCollision;
    //bool topOrBottomCollision;

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

        centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
        centerB = (Vector2){boxB.x + boxB.width / 2,boxB.y + boxB.height / 2}; 
        subtract = Vector2Subtract(centerA,centerB);
        halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
        halfWidthB  = (Vector2){ boxB.width*.5f, boxB.height*.5f };
        minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
        minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);

        if (CheckCollisionRecs(boxA, boxB)){
            collision = true;
        }else{
            collision = false;
        }

        if (minDistX < minDistY) {
            leftOrRightCollision = true;
        } else {
            leftOrRightCollision = false;
        } 

        /*
        //uncomment this to actually act on collision and move boxB
        //so then boxA is pushing boxA
        if (CheckCollisionRecs(boxA, boxB)){
            if (minDistX < minDistY) {
                //collide left or right
                boxB.x -= copysignf(minDistX, subtract.x); 
            } else {
                //collide top or bottom
                boxB.y -= copysignf(minDistY, subtract.y); 
            } 
        }
    */

        // Get rectangle of collision area
        if (collision) boxCollision = GetCollisionRec(boxA, boxB);

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision? RED : BLACK);

            DrawRectangleRec(boxA, GOLD);
            DrawRectangleRec(boxB, BLUE);
            //center points
            DrawCircle(centerA.x,centerA.y,10, RED); 
            DrawCircle(centerB.x,centerB.y,10, RED); 

            DrawText(TextFormat("CenterA : %.2f %.2f",centerA.x,centerA.y),210,90,30,RED);
            DrawText(TextFormat("CenterB : %.2f %.2f",centerB.x,centerB.y),210,120,30,RED);

            if (collision)
            {
                // collision area number value
                DrawText(TextFormat("Collision Area: %i", (int)boxCollision.width*(int)boxCollision.height), 210,50, 30, LIME);
                DrawText(TextFormat("Subtract : %.2f %.2f",subtract.x,subtract.y),210,160,30,BLACK);
                DrawText(TextFormat("minDist : %.2f %.2f",minDistX,minDistY),210,200,30,BLACK);
                // Draw green collision area
                DrawRectangleRec(boxCollision,LIME);

                // Draw collision message
                //DrawText("COLLISION!", GetScreenWidth()/2 - MeasureText("COLLISION!", 20)/2, screenUpperLimit/2 - 10, 20, BLACK);
                if (leftOrRightCollision){
                    DrawText("LEFT OR RIGHT COLLISION!", GetScreenWidth()/2 - MeasureText("LEFT OR RIGHT COLLISION!", 20)/2, screenUpperLimit/2 - 10, 20, BLACK);
                    //DrawText("Left or Right Collision",40,90,30,RED);
                }else{
                    DrawText("TOP OR BOTTOM COLLISION!", GetScreenWidth()/2 - MeasureText("TOP OR BOTTOM COLLISION!", 20)/2, screenUpperLimit/2 - 10, 20, BLACK);
                    //DrawText("Top or Bottom Collision",40,90,30,BLACK);
                }

            }

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
