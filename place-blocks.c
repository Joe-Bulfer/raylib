/*
gcc place-blocks.c -o place-blocks -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./place-blocks
*/

//now you can pick up sticks. Next integrate existing place block feature to place sticks
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h> // Required for: copysignf()
// debug x, y, font, color
#define DB_X 1090
#define DB_Y 20
#define DB_F 20
#define DB_CL                 (Color){ 155, 21, 41, 255 }

#define INVENTORY_GRAY        (Color){ 130, 130, 130, 100 }
#define TRANSPARENT_WHITE (Color){255, 255, 255, 128}
// semi-transparent white

#define TRANSPARENT_BLUE      (Color){ 0, 121, 241, 55 } //for cursor
#define G 500 
#define FPS 30 
#define PLAYER_START_X 330
#define PLAYER_START_Y 370

typedef struct Player {
	Rectangle rect;
	float speed;
	bool canJump;
}Player;

typedef struct EnvItem {
	Rectangle rect;
	Color color;
} EnvItem;

typedef struct Block {
	Rectangle rect;
	Texture2D tex;
} Block;

typedef struct InventorySlot {
	Rectangle rect;
	Texture2D tex;
} InventorySlot ;

//void checkCollisions(EnvItem *envItems, int envItemsLength, Player *player);
void checkCollisions(Block *blocks, int envItemsLength, Player *player);
//void checkCollisions(void *items, int envItemsLength, Player *player, size_t itemSize);

//void placeBlocks(EnvItem *envItems, int envItemsLength);
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height);

void UpdateActiveSlot();
void DrawInventory();

bool topCollision = false;

bool showDebugInfo = true;

int activeSlot = -1;
#define INVENTORY_SIZE 5
Texture2D sandBlock;

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "Block Placing Demo");
    Player player = { 0 };
    //start position
    player.rect = (Rectangle){ PLAYER_START_X, PLAYER_START_Y,30,30 }; 
    Vector2 position; //rect.x,rect.y
    player.speed = 0;
    player.canJump = false;
    bool pickaxeActive = false; 
    bool sandActive = false; 
    sandBlock = LoadTexture("rocky-sand-10x10.png");        
    InventorySlot inventory[INVENTORY_SIZE] = {
        {{30,30,80,80},sandBlock},
        {{130,30,80,80},sandBlock},
        {{230,30,80,80},sandBlock},
        {{330,30,80,80},sandBlock},
        {{430,30,80,80},sandBlock}
    };
    //InventorySlot sandSlot = {{30,30,80,80},sandBlock};
    //sandSlot.tex.width += 20;
    //sandSlot.tex.height += 20;
    //Texture2D playerTex = LoadTexture("cat_run.png");        
    Vector2 mousePos = {0,0}; //mouse hovers over chest
    int canPlace = 0;

    
    //static array
    #define MAX_BLOCKS 100
    int blockCount = 4;
    Block B_TX_Blocks[MAX_BLOCKS] = {
        {{  300, 400, 10, 10 }, sandBlock  }, //floor
        {{ 310, 400, 10, 10}, sandBlock   }, //floor
        {{ 320, 400, 10, 10}, sandBlock   }, //floor
        {{ 330, 400, 10, 10}, sandBlock   }, //floor
    };
    //if you want it dynamic
    /*
    Block *B_TX_Blocks = (Block*)malloc(sizeof(Block) * MAX_BLOCKS  );
    B_TX_Blocks[0] = (Block){{  300, 400, 10, 10 }, sandBlock  }; //floor
    B_TX_Blocks[1] = (Block){{ 310, 400, 10, 10}, sandBlock   }; //floor
    B_TX_Blocks[2] = (Block){{ 320, 400, 10, 10}, sandBlock   }; //floor
    B_TX_Blocks[3] = (Block){{ 330, 400, 10, 10}, sandBlock   }; //floor
*/
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

        //checkCollisions(bEnvItems, B_ENVITEMS, &player);
        //checkCollisions(bEnvItems, B_ENVITEMS, &player, sizeof(EnvItem));
        checkCollisions(B_TX_Blocks, MAX_BLOCKS, &player);


        
        // reset
        if (IsKeyPressed(KEY_R))
        {
                player.rect.x = PLAYER_START_X;
                player.rect.y = PLAYER_START_Y;
                player.speed = 0;
                player.speed = 0;
                camera.zoom = 3.0;
                blockCount = 4;
        }

        //frameRec.height = -(float)playerTex.height / 4;  // Negate to flip left
        if (IsKeyDown(KEY_A)){
            player.rect.x -= 3;
        }
        else if (IsKeyDown(KEY_D)){
            player.rect.x += 3;
        }


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

        if (IsKeyPressed(KEY_L)) showDebugInfo = !showDebugInfo ;
        // Draw
        //-----------------------------------------------------
        UpdateCameraCenter(&camera, &player, screenWidth, screenHeight);


        BeginDrawing();

	BeginMode2D(camera);
        //DrawTexture(backgroundTex,backgroundTex_xpos,-1000,WHITE);


            ClearBackground(BLACK);

            // environment
            //for (int i = 0; i < NB_ENVITEMS; i++) DrawRectangleRec(nbEnvItems[i].rect, nbEnvItems[i].color);
            //for (int i = 0; i < B_ENVITEMS; i++) DrawRectangleRec(bEnvItems[i].rect, bEnvItems[i].color);
            for (int i = 0; i < blockCount; i++) DrawTexture(B_TX_Blocks[i].tex, B_TX_Blocks[i].rect.x,B_TX_Blocks[i].rect.y,WHITE);

            //sand block test
            //DrawTexture(sandBlock, 365, 350, WHITE);
            mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
            //round to nice even number
            mousePos.x = floorf(mousePos.x / 10) * 10.0f;
            mousePos.y = floorf(mousePos.y / 10) * 10.0f;

            //transparent cursor
            if (activeSlot == 0){
                DrawTexture(inventory[activeSlot].tex,mousePos.x, mousePos.y,TRANSPARENT_WHITE);
                //DrawRectangle(mousePos.x, mousePos.y, 10, 10, TRANSPARENT_BLUE);

                if (IsMouseButtonDown(0) && blockCount < MAX_BLOCKS) {
                    canPlace = 1;
                    //don't let block place inside player
                    if (CheckCollisionRecs((Rectangle){mousePos.x, mousePos.y, 10, 10},player.rect)){
                        canPlace = 0;}
                    //could be made more efficient by not running this
                    //if canPlace is already 0
                    for (int i = 0; i < blockCount; i++) {
                        Vector2 envPos = (Vector2){B_TX_Blocks[i].rect.x, B_TX_Blocks[i].rect.y};
                        if (Vector2Equals(envPos, mousePos)) {
                            canPlace = 0;
                        }
                    }
                    
                    if (canPlace) {
                        Block newBlock = { 
                            (Rectangle){mousePos.x, mousePos.y, 10, 10},  // Position
                            sandBlock  // Texture
                        };

                        // Check if we need to expand the array
                                /*
                        if (blockCount >= MAX_BLOCKS) {
                            MAX_BLOCKS += 10;  // Expand in chunks
                            B_TX_Blocks = (Block*)realloc(B_TX_Blocks, sizeof(Block) * MAX_BLOCKS);
                        }
                                */

                        // Place the new block and increment count
                        B_TX_Blocks[blockCount] = newBlock;
                        blockCount++;  // Increment after placement
                    }
                }
        }
        //how do i mine or destory blocks?
        //else if (activeSlot ==1){

            

            
            //player
            DrawRectangle(position.x,position.y,30,30,RED);
            //DrawTextureRec(playerTex, frameRec, position, WHITE);  //  how do I scale this?
            //DrawRectangleRec(player.rect, BLUE); //old thing before animation
            

	EndMode2D();
        DrawFPS(10, 10);
        
        /*
        if (IsKeyPressed(KEY_ONE))
            sandActive = !sandActive ;
        else if (IsKeyPressed(KEY_TWO))
            pickaxeActive = !pickaxeActive ;
        */

        UpdateActiveSlot();
        DrawInventory(inventory);
        //DrawRectangleRec(sandSlot.rect,INVENTORY_GRAY);
        //DrawTexture(sandSlot.tex,sandSlot.rect.x+20,sandSlot.rect.y+20,WHITE);

        if (showDebugInfo){
            DrawText(TextFormat("player position : %.2f, %.2f",player.rect.x,player.rect.y),DB_X,DB_Y+25,DB_F,DB_CL);
            DrawText(TextFormat("player speed : %.2f",player.speed),DB_X,DB_Y+50,DB_F,DB_CL);
            DrawText(TextFormat("mousePos : %.2f,%.2f",mousePos.x,mousePos.y),DB_X,DB_Y+75,DB_F,DB_CL);
            DrawText(TextFormat("topCollision %d canJump - %d",topCollision,player.canJump),DB_X,DB_Y+100,DB_F,DB_CL);
            DrawText(TextFormat("activeSlot - %d",activeSlot ),DB_X,DB_Y+125,DB_F,DB_CL);
            //DrawText(TextFormat("framesCount - %.2f",framesCount),DB_X,DB_Y+25,DB_CL);
            //DrawText(TextFormat("currentFrame - %.2f",currentFrame),DB_X,DB_Y+150,DB_F,DB_CL);
            //DrawText(TextFormat("frameSpeed - %.2f",frameSpeed),DB_X,DB_Y+25,DB_CL);
            //DrawText(TextFormat("placeBlockMode  - %d",placeBlockMode  ),DB_X,DB_Y+175,DB_F,DB_CL);
            //DrawText(TextFormat("chestOpen: %d\nwithinChestRange: %d",chestOpen,withinChestRange),DB_X,DB_Y+200,DB_F,DB_CL);
            DrawText(TextFormat("MAX_BLOCKS: %d\nblockCount: %d",MAX_BLOCKS,blockCount),DB_X,DB_Y+250,DB_F,DB_CL);
        }
        
	
        EndDrawing();
        //-----------------------------------------------------
    }
    UnloadTexture(sandBlock);
    CloseWindow();        // Close window and OpenGL context
    return 0;
}

void checkCollisions( Block *blocks, int blocksLength, Player *player){
    
        Vector2 centerA, centerB;
        Vector2 subtract;
        Vector2 halfWidthA,halfWidthB;
        float minDistX, minDistY;  
        topCollision = false;
       
	// non blocking (NB) blocks to keep background it start of array
	for (int i = 0; i < blocksLength; i++)
	{
            Rectangle boxA = blocks[i].rect;
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
        
        
    }
}


void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height){
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target = (Vector2){player->rect.x,player->rect.y};

}
// Function to update the active slot based on key presses
void UpdateActiveSlot() {
    if (IsKeyPressed(KEY_ONE)) {
        activeSlot = 0;
    } 
    else if (IsKeyPressed(KEY_TWO)) {
        activeSlot = 1;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        activeSlot = 2;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        activeSlot = 3;
    }
    else if (IsKeyPressed(KEY_FIVE)) {
        activeSlot = 4;
    }
}

// Function to draw the inventory and highlight the active slot
void DrawInventory(InventorySlot *inventory) {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        // Highlight the active slot with a border or different color
        Color slotColor = (i == activeSlot) ? GOLD : INVENTORY_GRAY;  // Highlight active slot in GOLD

        // Draw the inventory slot with a background color
        DrawRectangleRec(inventory[i].rect, slotColor);

        // Draw the texture inside the slot
        DrawTexture(inventory[i].tex, inventory[i].rect.x + 20, inventory[i].rect.y + 20, WHITE);
    }
}
