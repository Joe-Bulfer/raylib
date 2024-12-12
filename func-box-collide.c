#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"

#include <stdlib.h> // Required for: copysignf()
// debug x, y, font, color
#define DB_X 1090
#define DB_Y 20
#define DB_F 20
#define DB_CL                 (Color){ 155, 21, 41, 255 }
#define TRANSPARENT_BLUE      (Color){ 0, 121, 241, 55 } //for cursor
#define G 500 
#define FPS 30 
#define PLAYER_START_X 130
#define PLAYER_START_Y 370
#define MIN_PLAYER_FRAME_SPEED 7 
#define MAX_PLAYER_FRAME_SPEED 10 //idk if this is a float or int
#define MIN_PLAYER_SPEED 70 // pixels/x grid coords traveled per second
#define MAX_PLAYER_SPEED 100 

// TEST -------------------
typedef struct {
    int frameCount; //my program doesn't seem to have this count variable
    int currentFrame;
    int frameDelay; //framesSpeed
    int currentDelay; //framesCounter
} Animation;

typedef struct { //in world before collected
    Rectangle rect;
    Color color;
    bool collected;
} Item;
#define MAX_STICKS 100 
Item sticksArray[MAX_STICKS]; 
int sticksCount = 0; //length of array

//will I need this?
typedef struct { //in inventory after collected
    Item item;
    int count;
} ItemSlot;
/*

void UpdateAnimation(Animation *anim) {
    anim->currentDelay++;
    if (anim->currentDelay >= anim->frameDelay) {
        anim->currentDelay = 0;
        anim->currentFrame++;
        if (anim->currentFrame >= anim->frameCount) anim->currentFrame = 0;
    }
}

        if (IsKeyDown(KEY_A)){
            moving = true;
            player.rect.x -= 3;
            frameRec.width = -(float)playerTex.width / 4;  // Negate to flip left
            frameRec.x = (float)(currentFrame + 1) * (float)playerTex.width / 4; 
        }
        else if (IsKeyDown(KEY_D)){
            moving = true;
            player.rect.x += 3;
            frameRec.width = (float)playerTex.width / 4;  // Flip it right
            frameRec.x = (float)(currentFrame + 1) * (float)playerTex.width / 4; 
        }
        else moving = false;
        //#######animation

        if (moving){
            framesCounter++;
            //framesSpeed += 0.8;
            //  currentDelay?        framesDelay?
            if (framesCounter >= (60/framesSpeed))
            {
                framesCounter = 0;
                currentFrame = (currentFrame + 1) % 4;

                frameRec.x = (float)currentFrame*(float)playerTex.width/4;
            }
        }
        else if (!moving && player.canJump){
            //framesSpeed = 1;
            currentFrame = 3;
            frameRec.x = (float)currentFrame * (float)playerTex.width / 4;  // Update frame rectangle for standing
        }

	if (IsKeyPressed(KEY_SPACE) && player.canJump)
	{
                topCollision = false;
		player.speed = -400;
		player.canJump = false;
	}

        
        if (!topCollision) {
                currentFrame = 2;
                frameRec.x = (float)(currentFrame) * (float)playerTex.width / 4; // Jump frame (index 2)
		player.rect.y += player.speed * deltaTime;
		player.speed += G*deltaTime;
		player.canJump = false;
        }
*/
// TEST END -------------------
 


typedef struct Player {
	Rectangle rect;
	float speed;
	bool canJump;
}Player;

typedef struct EnvItem {
	Rectangle rect;
	Color color;
} EnvItem;

void checkCollisions(EnvItem *envItems, int envItemsLength, Player *player);

//void placeBlocks(EnvItem *envItems, int envItemsLength);
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height);

bool topCollision = false;

bool showDebugInfo = true;

//items
//Item sticks = {{ 50, 397, 20, 10 }, BROWN };
/*
Item sticksArray[0] = {{ 20, 397, 20, 10 }, BROWN };
Item sticksArray[1] = {{ 50, 397, 20, 10 }, BROWN };
Item sticksArray[2] = {{ 310, 397, 20, 10 }, BROWN };
Item sticksArray[3] = {{ 300, 200, 20, 10 }, BROWN };
*/
Item trunk = {{ 90, 320, 25, 80 }, BROWN };
Item leaves = {{ 80, 300, 45, 80 }, GREEN };
int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area");

    Player player = { 0 };
    player.rect = (Rectangle){ PLAYER_START_X, PLAYER_START_Y,30,30 }; //start position
    Vector2 position; //rect.x,rect.y
    player.speed = 0;
    player.canJump = false;

    Texture2D playerTex = LoadTexture("cat_run.png");        
    playerTex.width += 30;
    playerTex.height += 15;
    //Rectangle frameRec = { 0.0f, 0.0f, (float)playerTex.width/4, (float)playerTex.height };
    Rectangle frameRec = { 0.0f, 0.0f, playerTex.width/4, playerTex.height };
    int currentFrame;
    float framesSpeed = 7; // start at minimum walking speed
    int framesCounter; //start at first frame 
    bool moving; //walking left or right with A or D

    EnvItem envItems[] = {
            {{ -1000, -100, 3000, 500 }, SKYBLUE }, //background
            {{ -1000, 400, 3000, 200 }, DARKGREEN }, //floor
        /* 3 pixel staggered vertically to climb steps
            {{ 160, 397, 10,10 }, GRAY }, //steps
            {{ 170, 394, 10,10 }, GRAY }, //steps
            {{ 180, 391, 10,10 }, GRAY }, //steps
            {{ 190, 388, 10,10 }, GRAY }, //steps
            {{ 200, 385, 10,10 }, GRAY }, //steps
            {{ 210, 382, 10,10 }, GRAY }, //steps
            {{ 220, 379, 10,10 }, GRAY }, //steps
            */
            {{ 300, 200, 400, 10 }, DARKBROWN }, //platforms
            {{ 250, 300, 100, 10 }, DARKBROWN },
            {{ 650, 300, 100, 10 }, DARKBROWN },

            //{{ 50, 397, 20, 10 }, BROWN }, //sticks test
    };

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);


    Camera2D camera = { 0 };
    camera.target = (Vector2){player.rect.x,player.rect.y};

    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    camera.zoom = 3.0f;
    SetTargetFPS(FPS);               // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
	float deltaTime = GetFrameTime();
        position = (Vector2){player.rect.x,player.rect.y}; 
        camera.zoom += ((float)GetMouseWheelMove()*0.10f);

        if (camera.zoom > 4.0f) camera.zoom = 4.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
        // Update
        //-----------------------------------------------------
        // Move box if not paused

        checkCollisions(envItems, envItemsLength, &player);
        
        // reset
        if (IsKeyPressed(KEY_R))
        {
                player.rect.x = PLAYER_START_X;
                player.rect.y = PLAYER_START_Y;
                player.speed = 0;
                player.speed = 0;
                camera.zoom = 3.0;
        }

        if (IsKeyDown(KEY_A)){
            moving = true;
            player.rect.x -= 3;
            frameRec.width = -(float)playerTex.width / 4;  // Negate to flip left
            frameRec.x = (float)(currentFrame + 1) * (float)playerTex.width / 4; 
        }
        else if (IsKeyDown(KEY_D)){
            moving = true;
            player.rect.x += 3;
            frameRec.width = (float)playerTex.width / 4;  // Flip it right
            frameRec.x = (float)(currentFrame + 1) * (float)playerTex.width / 4; 
        }
        else moving = false;
        //#######animation

        if (moving){
            framesCounter++;
            framesSpeed += 0.1;
            if (framesSpeed > MAX_PLAYER_FRAME_SPEED) framesSpeed = MAX_PLAYER_FRAME_SPEED;
            if (framesCounter >= (FPS/framesSpeed)) //next frame of 4
            {
                framesCounter = 0;
                currentFrame = (currentFrame + 1) % 4;

                frameRec.x = (float)currentFrame*(float)playerTex.width/4;
            }
        }
        else if (!moving && player.canJump){
            framesSpeed = MIN_PLAYER_FRAME_SPEED;
            currentFrame = 3;
            frameRec.x = (float)currentFrame * (float)playerTex.width / 4;  // Update frame rectangle for standing
        }

	if (IsKeyPressed(KEY_SPACE) && player.canJump)
	{
                topCollision = false;
		player.speed = -400;
		player.canJump = false;
	}

        
        if (!topCollision) {
                currentFrame = 2;
                frameRec.x = (float)(currentFrame) * (float)playerTex.width / 4; // Jump frame (index 2)
		player.rect.y += player.speed * deltaTime;
		player.speed += G*deltaTime;
		player.canJump = false;
        }
        
	else player.canJump = true;
        player.rect.y += 1; // wierd thing I have to do, I don't know why.
        //-----------------------------------------------------

        if (IsKeyPressed(KEY_I)) showDebugInfo = !showDebugInfo ;
        // Draw
        //-----------------------------------------------------
        UpdateCameraCenter(&camera, &player, screenWidth, screenHeight);


        BeginDrawing();

	BeginMode2D(camera);


            ClearBackground(BLACK);

            // environment
            for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);
            // non collectable items
            //if (CheckCollisionRecs(stick.rect, player.rect)) sticks.collected = true;
            DrawRectangleRec(trunk.rect, trunk.color);
            DrawRectangleRec(leaves.rect, leaves.color);
            // items
            //if (CheckCollisionRecs(sticks.rect, player.rect)) sticks.collected = true;
            //if (!sticks.collected) DrawRectangleRec(sticks.rect, sticks.color);
            // player
            DrawTextureRec(playerTex, frameRec, position, WHITE);  //  how do I scale this?
            //DrawRectangleRec(player.rect, BLUE); //old thing before animation

        // inventory - idk to put this before or after EndMode2D
        //if (sticks.collected) DrawText(("Sticks: 1"),10,10,20,BLUE);
        //if (sticks.collected) sticksCollected++;

	EndMode2D();
        DrawFPS(10, 10);
        if (showDebugInfo){
            DrawText(TextFormat("player position : %.2f, %.2f",player.rect.x,player.rect.y),DB_X,DB_Y+25,DB_F,DB_CL);
            DrawText(TextFormat("player speed : %.2f",player.speed),DB_X,DB_Y+50,DB_F,DB_CL);
            if (topCollision)DrawText(TextFormat("topCollision - true"),DB_X,DB_Y+75,DB_F,DB_CL);
            if (player.canJump)DrawText(TextFormat("canJump - true"),DB_X,DB_Y+100,DB_F,DB_CL);
            DrawText(TextFormat("Animation"),DB_X,DB_Y+125,DB_F,DB_CL);
            //DrawText(TextFormat("framesCount - %.2f",framesCount),DB_X,DB_Y+25,DB_CL);
            DrawText(TextFormat("currentFrame - %.2f",currentFrame),DB_X,DB_Y+150,DB_F,DB_CL);
            //DrawText(TextFormat("frameSpeed - %.2f",frameSpeed),DB_X,DB_Y+25,DB_CL);
            DrawText(TextFormat("framesCounter - %.2f",framesCounter),DB_X,DB_Y+175,DB_F,DB_CL);
        }
        // inventory
        //if (sticks.collected) DrawText(("Sticks: 1"),100,30,30,BLUE);
        EndDrawing();
        //-----------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

//void checkCollisions(Rectangle *boxA, Player *player){
void checkCollisions( EnvItem *envItems, int envItemsLength, Player *player){
    
        Vector2 centerA, centerB;
        Vector2 subtract;
        Vector2 halfWidthA,halfWidthB;
        float minDistX, minDistY;  
        topCollision = false;
       
	for (int i = 1; i < envItemsLength; i++)
	{
            Rectangle boxA = envItems[i].rect;
            if (CheckCollisionRecs(boxA, player->rect)){
                centerA = (Vector2){boxA.x + boxA.width / 2,boxA.y + boxA.height / 2};
                centerB = (Vector2){player->rect.x + player->rect.width / 2,player->rect.y + player->rect.height / 2}; 
                subtract = Vector2Subtract(centerA,centerB);
                halfWidthA = (Vector2){ boxA.width*.5f, boxA.height*.5f };
                halfWidthB  = (Vector2){ player->rect.width*.5f, player->rect.height*.5f };
                minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
                minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);
                
                if (minDistX < minDistY) {
                    //collide left or right
                    player->rect.x -= copysignf(minDistX, subtract.x); 
                } else {
                    //collide top or bottom
                    if (subtract.y > 0){
                        topCollision = true;
                        player->speed = 0;
                        //player->rect.y += 1; //prevent flickering
                    }
                    player->rect.y -= copysignf(minDistY, subtract.y); 
                    player->speed = 0; // bounce off bottom of object
                } 
            }
        
        
        /*
            else{
                topCollision = false;
            }
       */ 
    }
}
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height){
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target = (Vector2){player->rect.x,player->rect.y};

}
