/*
gcc grenade.c -o grenade -I ../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./grenade
*/

#include "raylib.h"
#include "raymath.h"
#include <math.h> //for fabs

#define PI 3.14159

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

float grenadeSpeedX = 0.0f;
float grenadeSpeedY = 0.0f;
Vector2 grenadePos = {0.0f, 0.0f};  // grenade initial position
#define GRENADE_RADIUS 5

/*
float grenadeSpeedX;
float grenadeSpeedY;
Vector2 grenadePos;
*/

void UpdateCameraCenter(Camera2D *camera, Player *player);
void PlayerControls(Player *player, EnvItem *blocks);
bool IsCollision(Player *player, EnvItem *blocks); //only used within PlsyerControls
//void throwGrenade(Player *player, EnvItem *blocks); //only used within PlsyerControls
void ThrowGrenade(Vector2 playerPos,Vector2 *grenadePos, float *grenadeSpeedX,float *grenadeSpeedY,float angle);
void CheckGrenadeCollision(Vector2 *circlePos,float grenadeRadius,float *grenadeSpeedX, float *grenadeSpeedY,Rectangle boxA);

const int screenWidth = 800;
const int screenHeight = 450;

Vector2 mousePos;
Vector2 centerV = {screenWidth / 2, screenHeight / 2};

float angleRad;
float angleDeg;

//for CheckGrenadeCollision
Vector2 centerA, centerB;
Vector2 subtract;
Vector2 halfWidthA,halfWidthB;
float minDistX, minDistY;  

// Timer variables
double startTime = 0.0;     // Time when the timer starts
double timerDuration = 3.0; // Duration of the timer (in seconds)
bool timerRunning = false;  // Whether the timer is running or not
double elapsedTime;
bool soundPlayed = false;  // To ensure sound only plays once

int main(void)
{
    InitWindow(screenWidth, screenHeight, "overhead camera shooter");

    InitAudioDevice();
    Sound boomSound = LoadSound("assets/boom.wav");
    Texture2D explosionTex = LoadTexture("assets/explosion.png");
    //Texture2D explosionTex = LoadTexture("assets/explosion-25-frame.png");
    
    Vector2 explosionOffset = { -50.0f, -50.0f };  // Move it up and to the left
    Vector2 explosionScale = { 0.25f, 0.25f };  // Shrink the texture by a fourth


    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };

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
            ThrowGrenade(player.position,&grenadePos, &grenadeSpeedX, &grenadeSpeedY,angleRad);
        }
        grenadeSpeedX *= .988; 
        grenadeSpeedY *= .988; 
        if (fabs(grenadeSpeedX) < 0.40) { grenadeSpeedX = 0; }
        if (fabs(grenadeSpeedY) < 0.40) { grenadeSpeedY = 0; }

        for (int i = 0; i < BLOCKS_LENGTH; i++){ 
            CheckGrenadeCollision(&grenadePos,GRENADE_RADIUS,&grenadeSpeedX,&grenadeSpeedY,blocks[i].rect);
        }
        /*
         if left or right collide:
            xvelocity = xvelocity * -1 
         if top or bottom collide:
            yvelocity = yvelocity * -1 
        */

        // Update and draw the cannongrenade
        if (grenadeSpeedX != 0 || grenadeSpeedY != 0) {
            grenadePos.x += grenadeSpeedX;
            grenadePos.y += grenadeSpeedY;
            DrawCircleV(grenadePos, GRENADE_RADIUS, GREEN);
        }

        if (timerRunning){
            elapsedTime = GetTime() - startTime; 
        }
                if (elapsedTime >= timerDuration)
        {
            //DrawTextureV(explosionTex, grenadePos, WHITE);
            DrawTextureEx(explosionTex, Vector2Add(grenadePos, explosionOffset), 0.0f, explosionScale.x, WHITE);
            if (!soundPlayed) {  // Ensure sound plays only once
                PlaySound(boomSound); // Play sound when timer exceeds 3 seconds
                soundPlayed = true;  // Mark sound as played
            }
            //elapsedTime = timerDuration;  // Ensure timer does not go negative
            //PlaySound(boomSound);
            // AnimateExplostion(explostionTex,player);
            //timerRunning = false;  // Stop the timer
        }

        // Draw the cannon on the player
        DrawRectanglePro((Rectangle){ player.position.x, player.position.y, cannonLength, 20 }, (Vector2){ 0, 10 }, angleDeg, GRAY);

        //draw player
        DrawCircleV(player.position, 20, ORANGE);

        EndMode2D();
        DrawText(TextFormat("elapsedTime: %.2f", elapsedTime), 50, 200, 20, RED);

        // DEBUG INFO
        DrawText(TextFormat("Raylib Version: %s\n", RAYLIB_VERSION), 50, 50, 30, RED);
        DrawText(TextFormat("mousePos: %f, %f", mousePos.x, mousePos.y), 50, 400, 20, RED);
        DrawText(TextFormat("angleDeg: %f", angleDeg), 50, 370, 20, RED);
        DrawText(TextFormat("position: %f, %f", player.position.x,player.position.y), 50, 340, 20, RED);
        DrawText(TextFormat("camera zoom: %f", camera.zoom), 50, 310, 20, RED);

        EndDrawing();
    }

    UnloadTexture(explosionTex);
    UnloadSound(boomSound);
    CloseAudioDevice();

    CloseWindow(); // Close window and OpenGL context
    return 0;
}

void UpdateCameraCenter(Camera2D *camera, Player *player)
{
    camera->offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera->target = player->position;
}


/*
 *Simpler version of PlayerControls without IsCollision.
 *Problem is with this if you have W and A at the same time
 *while moving along a block to your left, you don't move
 *at all, instead of moving up along the block.
*/
/*
void PlayerControls(Player *player, EnvItem *blocks)
{
    Vector2 originalPos = player->position;
    if (IsKeyDown(KEY_A)) player->position.x -= 4;
    else if (IsKeyDown(KEY_D)) player->position.x += 4;

    if (IsKeyDown(KEY_W)) player->position.y -= 4;
    else if (IsKeyDown(KEY_S)) player->position.y += 4;

    for (int i = 0; i < BLOCKS_LENGTH; i++) {
        if (CheckCollisionCircleRec(player->position, 20, blocks[i].rect)) {
            player->position = originalPos;
        }
    }
}

*/

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


//void throwGrenade(Vector2 grenadePos, float grenadeSpeedX,float grenadeSpeedY,EnvItem *blocks){
void ThrowGrenade(Vector2 playerPos,Vector2 *grenadePos, float *grenadeSpeedX,float *grenadeSpeedY,float angle){
    //*grenadePos = (Vector2){ 400, 280 };
    soundPlayed = false;
    *grenadePos = playerPos;
    float speed = 5.0f;
    *grenadeSpeedX = cos(angleRad) * speed;
    *grenadeSpeedY = sin(angleRad) * speed;
    //timer test
    startTime = GetTime();  // Start the timer when space bar is pressed
    timerRunning = true;
    //*grenadeSpeedX = 5.0f;
    //*grenadeSpeedY = 5.0f;
    //
}

void CheckGrenadeCollision(Vector2 *grenadePos,float grenadeRadius,float *grenadeSpeedX, float *grenadeSpeedY,Rectangle boxA){
        centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
        centerB = (Vector2){grenadePos->x,grenadePos->y}; 
        subtract = Vector2Subtract(centerA,centerB);
        halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
        halfWidthB  = (Vector2){ grenadeRadius*.5f, grenadeRadius*.5f };
        minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
        minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);
        
        // Check if collision occurs between grenade and boxA
        if (CheckCollisionCircleRec(*grenadePos, grenadeRadius, boxA)) {

            // If horizontal collision (left-right)
            if (minDistX < minDistY) {
                *grenadeSpeedX *= -0.8f;  // Reverse horizontal speed

                // Adjust grenade position to prevent it from getting stuck
                if (subtract.x > 0) {
                    grenadePos->x += minDistX;  // grenade is to the left of the box, move right
                } else {
                    grenadePos->x -= minDistX;  // grenade is to the right of the box, move left
                }
            }
            // If vertical collision (top-bottom)
            else {
                *grenadeSpeedY *= -0.8f;  // Reverse vertical speed

                // Adjust grenade position to prevent it from getting stuck
                if (subtract.y > 0) {
                    grenadePos->y += minDistY;  // grenade is below the box, move up
                } else {
                    grenadePos->y -= minDistY;  // grenade is above the box, move down
                }
            }
        } 
}
