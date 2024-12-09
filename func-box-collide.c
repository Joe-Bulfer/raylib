#include "raylib.h"
#include "raymath.h"

#include <stdlib.h> // Required for: copysignf()
#define G 500 

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

bool topCollision = false;

bool showDebugInfo = true;

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area");

    Player player = { 0 };
    player.rect = (Rectangle){ 100, 50,30,30 };
    player.speed = 0;
    player.canJump = false;

    EnvItem envItems[] = {
            {{ 410, 350, 200, 100 }, PINK }, 
            {{ 150, 370, 200, 100 }, RED }, 
            {{ 190, 170, 200, 100 }, RED }, 
            {{ 550, 470, 200, 100 }, BLUE }, 
    };

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
    // Moving box
    Rectangle boxA = { 10, GetScreenHeight()/2.0f - 50, 200, 100 };
    Rectangle testBox = { 310, 350, 200, 100 };
    int boxASpeedX = 4;

    // Mouse moved box
    //Rectangle boxB = { GetScreenWidth()/2.0f - 30, GetScreenHeight()/2.0f - 30, 60, 60 };

    int screenUpperLimit = 40;      // Top menu limits

    bool pause = true;             // Movement pause
    
    SetTargetFPS(30);               // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
	float deltaTime = GetFrameTime();
        // Update
        //-----------------------------------------------------
        // Move box if not paused
        if (!pause) boxA.x += boxASpeedX;

        // Bounce box on x screen limits
        if (((boxA.x + boxA.width) >= GetScreenWidth()) || (boxA.x <= 0)) boxASpeedX *= -1;

        // Make sure Box B does not go out of move area limits
        if ((player.rect.x + player.rect.width) >= GetScreenWidth()) player.rect.x = GetScreenWidth() - player.rect.width;
        else if (player.rect.x <= 0) player.rect.x = 0;

        if ((player.rect.y + player.rect.height) >= GetScreenHeight()) player.rect.y = GetScreenHeight() - player.rect.height;
        else if (player.rect.y <= screenUpperLimit) player.rect.y = (float)screenUpperLimit;

        
        checkCollisions(envItems, envItemsLength, &player);
        
        
        // Pause Box A movement
        if (IsKeyPressed(KEY_P)) pause = !pause;
        if (IsKeyPressed(KEY_R))
        {
                player.rect.x = 100;
                player.rect.y = 50;
                player.speed = 0;
        }

        if (IsKeyDown(KEY_A)) player.rect.x -= 3;
        if (IsKeyDown(KEY_D)) player.rect.x += 3;
	if (IsKeyPressed(KEY_SPACE) && player.canJump)
	{
                topCollision = false;
		player.speed = -400;
		player.canJump = false;
	}

        
        if (!topCollision) {
		player.rect.y += player.speed * deltaTime;
		player.speed += G*deltaTime;
		player.canJump = false;
        }
        
	else player.canJump = true;
        player.rect.y += 1; // wierd thing I have to do, I don't know why.
        //-----------------------------------------------------

        if (IsKeyDown(KEY_I)) showDebugInfo = !showDebugInfo ;
        // Draw
        //-----------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            //DrawRectangleRec(boxA, GOLD);
            //DrawRectangleRec(testBox, RED);
            DrawRectangleRec(player.rect, BLUE);
            for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

            if (showDebugInfo){
                DrawText(TextFormat("player speed : %.2f",player.speed),700,50,30,RED);
                if (topCollision)DrawText(TextFormat("topCollision - true"),700,80,30,RED);
                if (player.canJump)DrawText(TextFormat("canJump - true"),700,110,30,RED);
            }
        /*
            DrawText(TextFormat("CenterA : %.2f %.2f",centerA.x,centerA.y),200,50,30,RED);
            DrawText(TextFormat("CenterB : %.2f %.2f",centerB.x,centerB.y),200,80,30,RED);
            DrawText(TextFormat("Subtract : %.2f %.2f",subtract.x,subtract.y),200,110,30,RED);
            DrawText(TextFormat("minDist : %.2f %.2f",minDistX,minDistY),200,140,30,RED);
        */


            // Draw help instructions
            //DrawText("Press SPACE to PAUSE/RESUME", 20, screenHeight - 35, 20, LIGHTGRAY);

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

//void checkCollisions(Rectangle *boxA, Player *player){
void checkCollisions( EnvItem *envItems, int envItemsLength, Player *player){
    
        Vector2 centerA, centerB;
        Vector2 subtract;
        Vector2 halfWidthA,halfWidthB;
        float minDistX, minDistY;  
        topCollision = false;
       
	for (int i = 0; i < envItemsLength; i++)
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
