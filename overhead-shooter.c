/*
gcc overhead-shooter.c -o overhead-shooter -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./overhead-shooter
*/

#include "raylib.h"
#include "raymath.h"

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


float ballSpeedX = 0.0f;
float ballSpeedY = 0.0f;
Vector2 ballPos = {0.0f, 0.0f};  // Ball initial position

/*
float ballSpeedX;
float ballSpeedY;
Vector2 ballPos;
*/

void UpdateCameraCenter(Camera2D *camera, Player *player);
void PlayerControls(Player *player, EnvItem *blocks);
bool IsCollision(Player *player, EnvItem *blocks); //only used within PlsyerControls
//void FireCannon(Player *player, EnvItem *blocks); //only used within PlsyerControls
void FireCannon(Vector2 playerPos,Vector2 *ballPos, float *ballSpeedX,float *ballSpeedY,float angle);

const int screenWidth = 800;
const int screenHeight = 450;

Vector2 mousePos;
Vector2 centerV = {screenWidth / 2, screenHeight / 2};

float angleRad;
float angleDeg;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "overhead camera shooter");

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
            FireCannon(player.position,&ballPos, &ballSpeedX, &ballSpeedY,angleRad);
        }
        /*
         if left or right collide:
            xvelocity = xvelocity * -1 
         if top or bottom collide:
            yvelocity = yvelocity * -1 
        */

        // Update and draw the cannonball
        if (ballSpeedX != 0 || ballSpeedY != 0) {
            ballPos.x += ballSpeedX;
            ballPos.y += ballSpeedY;
            DrawCircleV(ballPos, 5, RED);
        }

        // Draw the cannon on the player
        DrawRectanglePro((Rectangle){ player.position.x, player.position.y, cannonLength, 20 }, (Vector2){ 0, 10 }, angleDeg, GRAY);

        //draw player
        DrawCircleV(player.position, 20, ORANGE);

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


//void FireCannon(Vector2 ballPos, float ballSpeedX,float ballSpeedY,EnvItem *blocks){
void FireCannon(Vector2 playerPos,Vector2 *ballPos, float *ballSpeedX,float *ballSpeedY,float angle){
    //*ballPos = (Vector2){ 400, 280 };
    *ballPos = playerPos;
    float speed = 5.0f;
    *ballSpeedX = cos(angleRad) * speed;
    *ballSpeedY = sin(angleRad) * speed;
    //*ballSpeedX = 5.0f;
    //*ballSpeedY = 5.0f;
    //
}
void MyCollisionCircleRec(Vector2 circlePos,float circleDiameter,Rectangle box){
}
