/*
gcc collision-bouncy-ball.c -o collision-bouncy-ball -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./collision-bouncy-ball
*/
#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>     // Required for: abs()

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Joe Bulfer collision-bouncy-ball");

    // Box A: Moving box
    Rectangle boxA = { 10, GetScreenHeight()/2.0f +30, 200, 100 };
    int boxASpeedX = 4;

    Vector2 mousePos;
    float turretRadius = 40;

    float ballSpeedX = 0.0f;
    float ballSpeedY = 0.0f;
    Vector2 ballPos = {0.0f, 0.0f};  // Ball initial position
    float ballRadius = 8;
    Vector2 centerV = {screenWidth / 2,100};

    float angleRad;
    float angleDeg;

    const float cannonLength = 60.0f;


    bool pause = false;             // Movement pause

    Vector2 centerA, centerB;
    Vector2 subtract;
    Vector2 halfWidthA,halfWidthB;
    float minDistX, minDistY;  

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

        
        centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
        centerB = (Vector2){ballPos.x,ballPos.y}; 
        subtract = Vector2Subtract(centerA,centerB);
        halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
        halfWidthB  = (Vector2){ ballRadius*.5f, ballRadius*.5f };
        minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
        minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);

        
        
        // Check if collision occurs between ball and boxA
        if (CheckCollisionCircleRec(ballPos, ballRadius, boxA)) {

            // If horizontal collision (left-right)
            if (minDistX < minDistY) {
                ballSpeedX *= -1.0f;  // Reverse horizontal speed

                // Adjust ball position to prevent it from getting stuck
                if (subtract.x > 0) {
                    ballPos.x += minDistX;  // Ball is to the left of the box, move right
                } else {
                    ballPos.x -= minDistX;  // Ball is to the right of the box, move left
                }
            }
            // If vertical collision (top-bottom)
            else {
                ballSpeedY *= -1.0f;  // Reverse vertical speed

                // Adjust ball position to prevent it from getting stuck
                if (subtract.y > 0) {
                    ballPos.y += minDistY;  // Ball is below the box, move up
                } else {
                    ballPos.y -= minDistY;  // Ball is above the box, move down
                }
            }
        } 

        // bounce off walls
        if ((ballPos.x >= (GetScreenWidth() - ballRadius)) || (ballPos.x <= ballRadius)) ballSpeedX *= -1.0f;
        if ((ballPos.y >= (GetScreenHeight() - ballRadius)) || (ballPos.y <= ballRadius)) ballSpeedY *= -1.0f;

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision? RED : BLACK);
            mousePos = GetMousePosition();
            angleRad = atan2(mousePos.y - centerV.y, mousePos.x - centerV.x);
            angleDeg = angleRad * (180 / PI); //convert to degrees

            
            //fire cannon
            if (IsMouseButtonDown(0)) {
                ballPos = centerV;
                float speed = 5.0f;
                ballSpeedX = cos(angleRad) * speed;
                ballSpeedY = sin(angleRad) * speed;
            }
            if (ballSpeedX != 0 || ballSpeedY != 0) {
                ballPos.x += ballSpeedX;
                ballPos.y += ballSpeedY;
                DrawCircleV(ballPos, ballRadius, RED);
            }
            // Draw the cannon on the player
            DrawRectanglePro((Rectangle){ centerV.x, centerV.y, cannonLength, 20 }, (Vector2){ 0, 10 }, angleDeg, GRAY);

            DrawRectangleRec(boxA, GOLD);
            DrawCircleV(centerV,turretRadius,BLUE);
            //center points
        
            // Draw help instructions
            DrawText("Press SPACE to PAUSE/RESUME", 20, screenHeight - 35, 20, LIGHTGRAY);

            DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}
