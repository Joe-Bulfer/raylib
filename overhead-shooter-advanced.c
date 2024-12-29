/*
ball now bounces
next make the ball slow down, shoot multiple, etc.
*/
/*
gcc overhead-shooter.c -o overhead-shooter -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./overhead-shooter
*/

#include "raylib.h"
#include "raymath.h"

//#define PI 3.14159

typedef struct Player {
    Vector2 position;
    Color color;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    Color color;
} EnvItem;

#define BLOCKS_LENGTH 4
EnvItem blocks[BLOCKS_LENGTH] = {
    {{80,580,330,80}, BLUE},
    {{190,80,100,200}, BLUE},
    {{490,280,50,200}, BLUE}
};


float ballSpeedX = 0.0f;
float ballSpeedY = 0.0f;
Vector2 ballPos = {0.0f, 0.0f};  // Ball initial position
const float ballRadius = 10; //bullet/cannonball

void UpdateCameraCenter(Camera2D *camera, Player *player);
void PlayerControls(Player *player, EnvItem *blocks);
bool IsCollision(Player *player, EnvItem *blocks); //only used within PlsyerControls
//void FireCannon(Player *player, EnvItem *blocks); //only used within PlsyerControls
void FireCannon(Vector2 playerPos,Vector2 *ballPos, float *ballSpeedX,float *ballSpeedY,float angle);
void CheckCollision(Vector2 *circlePos,float ballRadius,float *ballSpeedX, float *ballSpeedY,Rectangle boxA);

const int screenWidth = 800;
const int screenHeight = 450;

Vector2 mousePos;
Vector2 centerV = {screenWidth / 2, screenHeight / 2};

float angleRad;
float angleDeg;

//for collision
Vector2 centerA, centerB;
Vector2 subtract;
Vector2 halfWidthA,halfWidthB;
float minDistX, minDistY;  

int main(void)
{
    InitWindow(screenWidth, screenHeight, "overhead camera shooter");

    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    const float playerRadius = 20; 

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 2.0f;

    SetTargetFPS(60);

    const float cannonLength = 40.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        if (camera.zoom > 2.5f) camera.zoom = 2.5f;
        else if (camera.zoom < 1.50f) camera.zoom = 1.50f;

        PlayerControls(&player, blocks);
        UpdateCameraCenter(&camera, &player);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camera);

        //draw blocks
        for (int i = 0; i < BLOCKS_LENGTH; i++) DrawRectangleRec(blocks[i].rect, blocks[i].color);

        mousePos = GetMousePosition();

        // Calculate angle between the player/center of screen and the mouse position
        angleRad = atan2(mousePos.y - centerV.y, mousePos.x - centerV.x);
        angleDeg = angleRad * (180 / PI); //convert to degrees

        
        //fire cannon
        if (IsMouseButtonDown(0)) {
            FireCannon(player.position,&ballPos, &ballSpeedX, &ballSpeedY,angleRad);
        }

        // Update and draw the cannonball
        if (ballSpeedX != 0 || ballSpeedY != 0) {
            ballPos.x += ballSpeedX;
            ballPos.y += ballSpeedY;
            DrawCircleV(ballPos, 5, RED);
        }
        for (int i = 0; i < BLOCKS_LENGTH; i++){ 
            CheckCollision(&ballPos,ballRadius,&ballSpeedX,&ballSpeedY,blocks[i].rect);
        }

        // Draw the cannon on the player
        DrawRectanglePro((Rectangle){ player.position.x, player.position.y, cannonLength, 20 }, (Vector2){ 0, 10 }, angleDeg, GRAY);

        //draw player
        DrawCircleV(player.position, playerRadius, ORANGE);

        EndMode2D();

        // DEBUG INFO
        DrawText(TextFormat("Raylib Version: %s\n", RAYLIB_VERSION), 50, 50, 30, RED);
        DrawText(TextFormat("mousePos: %f, %f", mousePos.x, mousePos.y), 50, 400, 20, RED);
        DrawText(TextFormat("angleDeg: %f", angleDeg), 50, 370, 20, RED);
        DrawText(TextFormat("position: %f, %f", player.position.x,player.position.y), 50, 340, 20, RED);
        DrawText(TextFormat("camera zoom: %f", camera.zoom), 50, 310, 20, RED);

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
    return 0;
}

void UpdateCameraCenter(Camera2D *camera, Player *player)
{
    camera->offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera->target = player->position;
}


void PlayerControls(Player *player, EnvItem *blocks)
{
    Vector2 originalPos = player->position;

    // Try to move left
    if (IsKeyDown(KEY_A)) {
        player->position.x -= 4;
        if (IsCollision(player, blocks)) {
            player->position.x = originalPos.x; // Reset X if collision occurs
        }
    }
    
    // Try to move right
    if (IsKeyDown(KEY_D)) {
        player->position.x += 4;
        if (IsCollision(player, blocks)) {
            player->position.x = originalPos.x; // Reset X if collision occurs
        }
    }

    // Try to move up
    if (IsKeyDown(KEY_W)) {
        player->position.y -= 4;
        if (IsCollision(player, blocks)) {
            player->position.y = originalPos.y; // Reset Y if collision occurs
        }
    }

    // Try to move down
    if (IsKeyDown(KEY_S)) {
        player->position.y += 4;
        if (IsCollision(player, blocks)) {
            player->position.y = originalPos.y; // Reset Y if collision occurs
        }
    }
}

// only used within PlayerControls on each key WASD
bool IsCollision(Player *player, EnvItem *blocks)
{
    for (int i = 0; i < BLOCKS_LENGTH; i++) {
        if (CheckCollisionCircleRec(player->position, 20, blocks[i].rect)) {
            return true; // Collision detected
        }
    }
    return false; // No collision detected
}


void FireCannon(Vector2 playerPos,Vector2 *ballPos, float *ballSpeedX,float *ballSpeedY,float angle){
    *ballPos = playerPos;
    float speed = 5.0f;
    *ballSpeedX = cos(angleRad) * speed;
    *ballSpeedY = sin(angleRad) * speed;
}
void CheckCollision(Vector2 *ballPos,float ballRadius,float *ballSpeedX, float *ballSpeedY,Rectangle boxA){
        centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
        centerB = (Vector2){ballPos->x,ballPos->y}; 
        subtract = Vector2Subtract(centerA,centerB);
        halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
        halfWidthB  = (Vector2){ ballRadius*.5f, ballRadius*.5f };
        minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
        minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);
        
        // Check if collision occurs between ball and boxA
        if (CheckCollisionCircleRec(*ballPos, ballRadius, boxA)) {

            // If horizontal collision (left-right)
            if (minDistX < minDistY) {
                *ballSpeedX *= -1.0f;  // Reverse horizontal speed

                // Adjust ball position to prevent it from getting stuck
                if (subtract.x > 0) {
                    ballPos->x += minDistX;  // Ball is to the left of the box, move right
                } else {
                    ballPos->x -= minDistX;  // Ball is to the right of the box, move left
                }
            }
            // If vertical collision (top-bottom)
            else {
                *ballSpeedY *= -1.0f;  // Reverse vertical speed

                // Adjust ball position to prevent it from getting stuck
                if (subtract.y > 0) {
                    ballPos->y += minDistY;  // Ball is below the box, move up
                } else {
                    ballPos->y -= minDistY;  // Ball is above the box, move down
                }
            }
        } 
}
