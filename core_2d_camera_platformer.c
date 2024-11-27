/*
 * gcc core_2d_camera_platformer.c -o test -I/app/raylib/src -L/app/raylib/build -lraylib -lm -lpthread -ldl -lX11 && ./test
 *
 * notes:
 *
 * keep player Rectangle, Texture would not appear and melts through floor
 *
 * todo:
 * make reset clear blocks - works with quirk
 * made block place on keyup
 * 
 */

#define TRANSPARENT_BLUE      (Color){ 0, 121, 241, 55 }

#include "raylib.h"
#include "raymath.h"

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


void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);


int main(void)
{
	// ##### my stuff
	int maxBlocks = 1000;  // Max number of rectangles to store
	int blockCount = 0;  // Current number of rectangles
	//Vector2* clickedPositions = (Vector2*)malloc(maxblocks * sizeof(Vector2));
	Vector2 clickedPositions[maxBlocks];
	// ##### end my stuff
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

	Player player = { 0 };
	player.position = (Vector2){ 400, 280 };
	// test
	//Vector2 testval = Vector2Lerp(player.position,player.position,1.0);
	//so above func from raymath.h works, was Vector2Equals removed then? 
	player.speed = 0;
	player.canJump = false;
	EnvItem envItems[] = {
		{{ 0, 0, 1000, 400 }, 0, PINK }, //background
		{{ 0, 400, 1000, 200 }, 1, GRAY },
		{{ 300, 200, 400, 10 }, 1, GRAY },
		{{ 250, 300, 100, 10 }, 1, GRAY },
		{{ 650, 300, 100, 10 }, 1, GRAY },
		{{ 600, 350, 100, 40 }, 1, RED }
	};

	int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

	Camera2D camera = { 0 };
	camera.target = player.position;
	camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 2.0f;

	// Store pointers to the multiple update camera functions
	void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
		UpdateCameraCenter,
		UpdateCameraCenterInsideMap,
		UpdateCameraCenterSmoothFollow,
		UpdateCameraEvenOutOnLanding,
		UpdateCameraPlayerBoundsPush
	};

	int cameraOption = 1;
	int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

	char *cameraDescriptions[] = {
		"Follow player center",
		"Follow player center, but clamp to map edges",
		"Follow player center; smoothed",
		"Follow player center horizontally; updateplayer center vertically after landing",
		"Player push camera on getting too close to screen edge"
	};

	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------
	int clickedX = -1;  // Initialize clicked position
	int clickedY = -1;

	// Main game loop
	while (!WindowShouldClose())
	{
		// Update
		//----------------------------------------------------------------------------------
		float deltaTime = GetFrameTime();

		UpdatePlayer(&player, envItems, envItemsLength, deltaTime);

		camera.zoom += ((float)GetMouseWheelMove()*0.05f);

		if (camera.zoom > 3.0f) camera.zoom = 3.0f;
		else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

		if (IsKeyPressed(KEY_R))
		{
			camera.zoom = 2.0f;
			player.position = (Vector2){ 400, 280 };
			blockCount = 0;
			//both of these don't work, there is always one block visible after reset
			//memset(clickedPositions, 0, sizeof(clickedPositions));
			//for (int i = 0; i < maxBlocks; i++) {
			//clickedPositions[i] = (Vector2){-9999, -9999};  
			//}
		}

		if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1)%cameraUpdatersLength;

		// Call update camera function by its pointer
		cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(BLACK);

		BeginMode2D(camera);

		for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

		Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
		DrawRectangleRec(playerRect, RED);


		//test begin
		//if click, get x and y of click, 
		Vector2 mapGrid = GetScreenToWorld2D(GetMousePosition(), camera);
		//turns 400.2342346562 into 400.000000
		mapGrid.x = floorf(mapGrid.x / 10) * 10.0f;
		mapGrid.y = floorf(mapGrid.y / 10) * 10.0f;

		//transparent cursor
		DrawRectangle(mapGrid.x, mapGrid.y, 10, 10, TRANSPARENT_BLUE);

		//is mouse down, within maxBlocks, and mapGrid/clickedPos not in clickedPositions, then store in clickedPositions 
		//how do I compare mapGrid to clickedPositions[i]? Found Vector2Equals below, but it's still an "implicit declaration"? 
		//https://github.com/raysan5/raylib/blob/master/src/raymath.h
		if (IsMouseButtonDown(0) && blockCount < maxBlocks) {
			int alreadyClicked = 0;
			Vector2 clickedPos = { mapGrid.x, mapGrid.y };
			for (int i = 0; i < blockCount; i++) {
				//if (Vector2Equals(clickedPositions[i], clickedPos)) {
					alreadyClicked = 1;  // Position already taken
					break;
				//}
			}

				// Store the clicked position 
				clickedPositions[blockCount] = mapGrid;  
				blockCount++;  
			//}
		}

		// Draw previously clicked positions
		for (int i = 0; i < blockCount; i++) {
			DrawRectangle(clickedPositions[i].x, clickedPositions[i].y, 10, 10, BLUE);
		}
		//test end


		EndMode2D();
		//DEBUG INFO
		DrawText(TextFormat("Raylib Version: %s\n", RAYLIB_VERSION),50,50,30,RED);
		DrawText(TextFormat("Cursor: %f %f",mapGrid.x,mapGrid.y), 80, 400, 20, RED);
		DrawText(TextFormat("Blocks: %i",blockCount), 80, 420, 20, RED);

		DrawText("Controls:", 20, 20, 20, BLACK);
		DrawText("- Right/Left to move", 40, 40, 20, DARKGRAY);
		DrawText("- Space to jump", 40, 60, 20, DARKGRAY);
		DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 20, DARKGRAY);
		DrawText("- C to change camera mode", 40, 100, 20, DARKGRAY);
		DrawText("Current camera mode:", 20, 120, 20, BLACK);
		DrawText(cameraDescriptions[cameraOption], 40, 140, 20, DARKGRAY);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//-------------------------------------n-------------------------------------------------

	return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
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

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	camera->target = player->position;
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

	for (int i = 0; i < envItemsLength; i++)
	{
		EnvItem *ei = envItems + i;
		minX = fminf(ei->rect.x, minX);
		maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
		minY = fminf(ei->rect.y, minY);
		maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
	}

	Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
	Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);

	if (max.x < width) camera->offset.x = width - (max.x - width/2);
	if (max.y < height) camera->offset.y = height - (max.y - height/2);
	if (min.x > 0) camera->offset.x = width/2 - min.x;
	if (min.y > 0) camera->offset.y = height/2 - min.y;
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	static float minSpeed = 30;
	static float minEffectLength = 10;
	static float fractionSpeed = 0.8f;

	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	Vector2 diff = Vector2Subtract(player->position, camera->target);
	float length = Vector2Length(diff);

	if (length > minEffectLength)
	{
		float speed = fmaxf(fractionSpeed*length, minSpeed);
		camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
	}
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	static float evenOutSpeed = 700;
	static int eveningOut = false;
	static float evenOutTarget;

	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target.x = player->position.x;

	if (eveningOut)
	{
		if (evenOutTarget > camera->target.y)
		{
			camera->target.y += evenOutSpeed*delta;

			if (camera->target.y > evenOutTarget)
			{
				camera->target.y = evenOutTarget;
				eveningOut = 0;
			}
		}
		else
		{
			camera->target.y -= evenOutSpeed*delta;

			if (camera->target.y < evenOutTarget)
			{
				camera->target.y = evenOutTarget;
				eveningOut = 0;
			}
		}
	}
	else
	{
		if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
		{
			eveningOut = 1;
			evenOutTarget = player->position.y;
		}
	}
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
	static Vector2 bbox = { 0.2f, 0.2f };

	Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*width, (1 - bbox.y)*0.5f*height }, *camera);
	Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*width, (1 + bbox.y)*0.5f*height }, *camera);
	camera->offset = (Vector2){ (1 - bbox.x)*0.5f * width, (1 - bbox.y)*0.5f*height };

	if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
	if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
	if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
	if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}
