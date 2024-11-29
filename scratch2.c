
/*
 * gcc raria.c -o ../bin/Debug/raria -I ../build/external/raylib-master/src/ -L ../bin/Debug/ -lraylib -lm -lpthread -ldl -lX11
 */

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
	bool canJump;
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
	int maxBlocks = 1000;  // Max number of rectangles to store
	int blockCount = 0;  // Current number of rectangles
	Vector2 blocks[maxBlocks];
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	int showControls = 0;
	int showDebugInfo = 0;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

	Player player = { 0 };
	player.position = (Vector2){ 400, 280 };
	player.speed = 0;
	player.canJump = false;

	int envItemsLength = 6;
	EnvItem *envItems = (EnvItem*)malloc(sizeof(EnvItem) * envItemsLength);

	envItems[0] = (EnvItem){{ 0, 0, 1000, 400 }, 0, PINK };  // background
	envItems[1] = (EnvItem){{ 0, 400, 1000, 200 }, 1, GRAY };
	envItems[2] = (EnvItem){{ 300, 200, 400, 10 }, 1, GRAY };
	envItems[3] = (EnvItem){{ 250, 300, 100, 10 }, 1, GRAY };
	envItems[4] = (EnvItem){{ 650, 300, 100, 10 }, 1, GRAY };
	envItems[5] = (EnvItem){{ 600, 350, 100, 40 }, 1, RED };


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

		for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

		Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
		DrawRectangleRec(playerRect, RED);


		//test begin
		//if click, get x and y of click, 
		Vector2 clickedPos = GetScreenToWorld2D(GetMousePosition(), camera);
		//turns 400.2342346562 into 400.000000
		clickedPos.x = floorf(clickedPos.x / 10) * 10.0f;
		clickedPos.y = floorf(clickedPos.y / 10) * 10.0f;

		//transparent cursor
		DrawRectangle(clickedPos.x, clickedPos.y, 10, 10, TRANSPARENT_BLUE);

		//is mouse down, within maxBlocks, and clickedPos/clickedPos not in blocks, then store in blocks 
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


		EndMode2D();
		DrawText(TextFormat("TEST %i", canPlace),25,300,30,RED);

		// DEBUG INFO AND CONTROLS
		if (IsKeyPressed(KEY_P)) showDebugInfo = (showDebugInfo + 1)%2;//toggle from 0 to 1
		if (showDebugInfo){
			DrawText(TextFormat("Raylib Version: %s\n", RAYLIB_VERSION),25,380,30,RED);
			DrawText(TextFormat("Cursor: %f %f",clickedPos.x,clickedPos.y), 25, 410, 20, RED);
			DrawText(TextFormat("Blocks: %i",blockCount), 25, 430, 20, RED);
		}

		if (IsKeyPressed(KEY_O)) showControls = (showControls + 1)%2;//toggle from 0 to 1
		if (showControls){
			DrawText("Controls:", 20, 20, 20, BLACK);
			DrawText("- Right/Left to move", 40, 40, 20, DARKGRAY);
			DrawText("- Space to jump", 40, 60, 20, DARKGRAY);
			DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 20, DARKGRAY);
		}

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	free(envItems);

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//-------------------------------------n-------------------------------------------------

	return 0;
}

//void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, Vector2 *blocks, int blockCount, float delta)
{
	if (IsKeyDown(KEY_A)) player->position.x -= PLAYER_HOR_SPD*delta;
	if (IsKeyDown(KEY_D)) player->position.x += PLAYER_HOR_SPD*delta;
	if (IsKeyDown(KEY_SPACE) && player->canJump)
	{
		player->speed = -PLAYER_JUMP_SPD;
		player->canJump = false;
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
		player->canJump = false;
	}
	else player->canJump = true;
}

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target = player->position;
}

