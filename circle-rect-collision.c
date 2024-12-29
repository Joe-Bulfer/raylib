/*
gcc circle-rect-collision.c -o circle-rect-collision -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./circle-rect-collision
*/
#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>     // Required for: abs()

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Joe Bulfer Collision Example");

    // Box A: Moving box
    Rectangle boxA = { 10, GetScreenHeight()/2.0f +30, 200, 100 };
    int boxASpeedX = 4;

    Vector2 circlePos;
    float circleRadius = 30;

    int screenUpperLimit = 40;      // Top menu limits

    bool pause = false;             // Movement pause
    bool collision = false;         // Collision detection

    Vector2 centerA, centerB;
    Vector2 subtract;
    Vector2 halfWidthA,halfWidthB;
    float minDistX, minDistY;  
    bool leftOrRightCollision;

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

        // Update player-controlled-circle
        circlePos.x = GetMouseX();
        circlePos.y = GetMouseY();

        centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
        centerB = (Vector2){circlePos.x,circlePos.y}; 
        subtract = Vector2Subtract(centerA,centerB);
        halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
        halfWidthB  = (Vector2){ circleRadius*.5f, circleRadius*.5f };
        minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
        minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);

        
        if (CheckCollisionCircleRec(circlePos,circleRadius,boxA)){
            collision = true;
        }else{
            collision = false;
        }

        if (minDistX < minDistY) {
            leftOrRightCollision = true;
        } else {
            leftOrRightCollision = false;
        } 
        

        //uncomment this to actually act on collision and move the circle
        //so then boxA is pushing circle
        /*
        if (CheckCollisionCircleRec(circlePos,circleRadius,boxA)){
            if (minDistX < minDistY) {
                //collide left or right
                circlePos.x -= copysignf(minDistX, subtract.x); 
            } else {
                //collide top or bottom
                circlePos.y -= copysignf(minDistY, subtract.y); 
            } 
        }
        */

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision? RED : BLACK);

            DrawRectangleRec(boxA, GOLD);
            DrawCircleV(circlePos,circleRadius,BLUE);
            //center points
            DrawCircle(centerA.x,centerA.y,10, RED); 
            DrawCircle(centerB.x,centerB.y,10, RED); 

            DrawText(TextFormat("Rectangle: %.2f %.2f",centerA.x,centerA.y),210,90,30,RED);
            DrawText(TextFormat("Circle: %.2f %.2f",centerB.x,centerB.y),210,120,30,RED);

            if (collision)
            {
                DrawText(TextFormat("Subtract : %.2f %.2f",subtract.x,subtract.y),210,160,30,BLACK);
                DrawText(TextFormat("minDist : %.2f %.2f",minDistX,minDistY),210,200,30,BLACK);

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
