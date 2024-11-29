#define TRANSPARENT_BLUE      (Color){ 0, 121, 241, 55 }

#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"

#define G 500 
#define PLAYER_JUMP_SPD 350.0f //increases jump height
#define PLAYER_HOR_SPD 200.0f

typedef struct Player {
    Vector2 position;
    float speed;
    bool standing; //standing
    bool flipped; //turned left
    Texture2D texture;        // Texture for player (cat)
    int numFrames;            // Number of frames in the animation
    float frameTimer;         // Timer for animation frame switching
    int currentFrame;         // Current frame index in the animation
    float frameLength;        // Time length for each frame
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;


void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, Vector2 *blocks, int blockCount, float delta);
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

int main(void)
{
    float startTime = 0.0f; //to find time at win
    int win = 0;
    int maxBlocks = 1000;  // Max number of rectangles to store
    int blockCount = 0;  // Current number of rectangles
    Vector2 blocks[maxBlocks];

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    int showControls = 1;
    int showDebugInfo = 0;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.standing = false;

    // Load cat animation texture
    player.texture = LoadTexture("cat_run.png");
    player.numFrames = 4;  // 4 frames in the sprite sheet
    player.frameTimer = 0.0f;
    player.currentFrame = 0;
    player.frameLength = 0.1f; // Time per frame

    int envItemsLength = 6;
    EnvItem *envItems = (EnvItem*)malloc(sizeof(EnvItem) * envItemsLength);

    envItems[0] = (EnvItem){{ 0, 0, 1000, 400 }, 0, PURPLE };  // BACKGROUND
    envItems[1] = (EnvItem){{ 50, 50, 50, 50 }, 0, GREEN };  // GOAL
    envItems[2] = (EnvItem){{ 0, 400, 1000, 200 }, 1, GRAY };
    envItems[3] = (EnvItem){{ 300, 200, 400, 10 }, 1, GRAY };
    envItems[4] = (EnvItem){{ 250, 300, 100, 10 }, 1, GRAY };
    envItems[5] = (EnvItem){{ 650, 300, 100, 10 }, 1, GRAY };

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        UpdatePlayer(&player, envItems, envItemsLength, blocks, blockCount, deltaTime);

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 2.0f;
            player.position = (Vector2){ 400, 280 };
            blockCount = 0;
        }

        UpdateCameraCenter(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);

        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camera);

        // Draw environment items
        for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

        // Update and draw the animated player (cat)
        float playerWidth = (float)player.texture.width;
        float playerHeight = (float)player.texture.height;

        // Update animation frame
        player.frameTimer += GetFrameTime();
        if (player.frameTimer > player.frameLength)
        {
            player.currentFrame += 1;
            player.frameTimer = 0;

            if (player.currentFrame == player.numFrames)
            {
                player.currentFrame = 0;
            }
        }

        Rectangle playerSource = { 
            (float)player.currentFrame * playerWidth / (float)player.numFrames,
            0, 
            playerWidth / (float)player.numFrames, 
            playerHeight
        };

        Rectangle playerDest = {
            player.position.x-20,
            player.position.y-65,
            playerWidth * 4 / (float)player.numFrames,
            playerHeight * 4
        };

        DrawTexturePro(player.texture, playerSource, playerDest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        //COLISSION ----------
        Vector2 clickedPos = GetScreenToWorld2D(GetMousePosition(), camera);
        clickedPos.x = floorf(clickedPos.x / 10) * 10.0f;
        clickedPos.y = floorf(clickedPos.y / 10) * 10.0f;
        DrawRectangle(clickedPos.x, clickedPos.y, 10, 10, TRANSPARENT_BLUE);

        int canPlace = 0;
        if (IsMouseButtonDown(0) && blockCount < maxBlocks) {
                canPlace = 1;
                for (int i = 0; i < envItemsLength; i++) {
                        Vector2 envPos = (Vector2){envItems[i].rect.x, envItems[i].rect.y};
                        if (Vector2Equals(envPos, clickedPos)) {
                                canPlace = 0;
                        }
                }
                if(canPlace){
                        EnvItem newBlock = { 
                            (Rectangle){clickedPos.x, clickedPos.y, 10, 10},  // Position
                            1,  // Blocking
                            BLUE  // Color
                                };
                // Reallocate memory to add the new block
                envItemsLength++;  // Increase the length
                envItems = (EnvItem*)realloc(envItems, sizeof(EnvItem) * envItemsLength);

                // Append the new block
                envItems[envItemsLength - 1] = newBlock;
                blockCount++;
            }
        }
        //COLISSION END----------

        // Test for winning condition (if player collides with the goal)
        if (CheckCollisionRecs((Rectangle){ player.position.x - 10, player.position.y - 20, 20, 20 }, envItems[1].rect)) {
            win = 1;
        }

        EndMode2D();
        if (win){
            DrawText(TextFormat("YOU WON"),screenWidth/2-60,screenHeight/2,30,GREEN);
        }

        // Debug info and controls
        if (IsKeyPressed(KEY_P)) showDebugInfo = (showDebugInfo + 1)%2; // Toggle debug info
        if (showDebugInfo){
            DrawText(TextFormat("Raylib Version: %s\n", RAYLIB_VERSION), 25, 380, 30, RED);
            DrawText(TextFormat("Cursor: %f %f",clickedPos.x,clickedPos.y), 25, 410, 20, RED);
            DrawText(TextFormat("Blocks: %i",blockCount), 25, 430, 20, RED);
        }

        if (IsKeyPressed(KEY_O)) showControls = (showControls + 1)%2; // Toggle controls
        if (showControls){
            DrawText("GET TO THE GREEN SQUARE:", 10, 20, 20, LIGHTGRAY);
            DrawText("Controls:", 20, 40, 20, BLACK);
            DrawText("- Right/Left to move", 20, 60, 20, DARKGRAY);
            DrawText("- Space to jump", 20, 80, 20, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out, R to reset", 20, 100, 20, DARKGRAY);
            DrawText("- O to remove these instructions", 20, 120, 20, DARKGRAY);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    UnloadTexture(player.texture); // Unload player texture
    free(envItems);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, Vector2 *blocks, int blockCount, float delta)
{
	if (IsKeyDown(KEY_A)) player->position.x -= PLAYER_HOR_SPD*delta;
	if (IsKeyDown(KEY_D)) player->position.x += PLAYER_HOR_SPD*delta;
	if (IsKeyDown(KEY_SPACE) && player->standing)
	{
		player->speed = -PLAYER_JUMP_SPD;
		player->standing = false;
	}

	int hitObstacle = 0;
	for (int i = 0; i < envItemsLength; i++)
	{
		EnvItem *ei = envItems + i;
		Vector2 *p = &(player->position);
		if (ei->blocking &&
				ei->rect.x <= p->x &&
				ei->rect.x + ei->rect.width >= p->x &&
				ei->rect.y >= p->y &&
				ei->rect.y < p->y + player->speed*delta)
		{
			hitObstacle = 1;
			player->speed = 0.0f;
			p->y = ei->rect.y;
		}
	}

	if (!hitObstacle)
	{
		player->position.y += player->speed*delta;
		player->speed += G*delta;
		player->standing = false;
	}
	else player->standing = true;
}


void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target = player->position;
}

