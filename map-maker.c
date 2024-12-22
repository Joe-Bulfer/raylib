/*
gcc map-maker.c -o map-maker -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./map-maker
*/

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define DB_X 1000
#define DB_Y 20
#define DB_F 20
#define DB_CL                 (Color){ 155, 21, 41, 255 }
#define FPS 30 
#define TRANSPARENT_WHITE (Color){255, 255, 255, 128}
#define ERASER_PINK (Color){255, 105, 180, 128}

typedef struct Player {
    Rectangle rect;
} Player;

typedef struct Block {
    Rectangle rect;
    Texture2D tex;
} Block;

typedef struct InventorySlot {
    Rectangle rect;
    Texture2D tex;
    int texID;
} InventorySlot;

int editMode = 1;

int canPlace = 0;
Vector2 mousePos;
int activeSlot = 0;
#define INVENTORY_SIZE 5
#define MAX_BLOCKS 90
Block blocks[MAX_BLOCKS];
int blockCount = 0;

// Function declarations
void PlayerControls(Player *player);
void PlayerControlsWithCollision(Player *player, Block *envItems, int envItemsLength);
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height);
void UpdateActiveSlot();
void DrawInventory(InventorySlot *inventory);
void PlaceBlocks(Vector2 mousePos, InventorySlot *inventory, Block *blocks, int *blockCount);
void RemoveBlock(Vector2 mousePos, Block *blocks, int *blockCount);
void SaveMap(Block *blocks, int blockCount);
void checkCollisions( Block *envItems, int envItemsLength, Player *player);

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "Place Shape Game");

    Player player = { 0 };
    player.rect = (Rectangle){ 300, 300, 30, 30 }; 

    Texture2D sandTex = LoadTexture("rocky-sand-10x10.png");        
    Texture2D dirtTex = LoadTexture("dirt-10x10.png");        
    Texture2D grassTex = LoadTexture("grass-10x10.png");        
    Texture2D waterTex = LoadTexture("water-10x10.png");        
    Texture2D eraserTex = LoadTexture("eraser-10x10.png");        

    InventorySlot inventory[INVENTORY_SIZE] = {
        {{30, 30, 80, 80}, sandTex},
        {{130, 30, 80, 80}, dirtTex},
        {{230, 30, 80, 80}, grassTex},
        {{330, 30, 80, 80}, waterTex },
        {{430, 30, 80, 80}, eraserTex }
    };

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.rect.x, player.rect.y };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 3.0f;

    SetTargetFPS(FPS); // Set our game to run at 30 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        camera.zoom += ((float)GetMouseWheelMove() * 0.10f);
        if (camera.zoom > 4.0f) camera.zoom = 4.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R)) camera.zoom = 3.0;
        if (IsKeyPressed(KEY_P)) editMode = !editMode ;


        UpdateCameraCenter(&camera, &player, screenWidth, screenHeight);

        // Update the active inventory slot
        UpdateActiveSlot();

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        
        // Draw the placed blocks
        for (int i = 0; i < blockCount; i++) {
            DrawTexture(blocks[i].tex, blocks[i].rect.x, blocks[i].rect.y, WHITE);
        }

        mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        mousePos.x = floorf(mousePos.x / 10) * 10.0f;
        mousePos.y = floorf(mousePos.y / 10) * 10.0f;

        if (!editMode){
            player.rect.y += 1;//gravity
            PlayerControlsWithCollision(&player,blocks,blockCount);
            DrawRectangleRec(player.rect,RED);

            for (int i = 0;i<blockCount;i++){
                checkCollisions(&blocks[i],blockCount,&player);
            }
        }

        if (editMode){
            PlayerControls(&player);

            switch(activeSlot )
                {
                case 0: 
                    PlaceBlocks(mousePos, inventory, blocks, &blockCount);
                    DrawTexture(sandTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
                    break;
                case 1:
                    PlaceBlocks(mousePos, inventory, blocks, &blockCount);
                    DrawTexture(dirtTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
                    break;
                case 2:
                    PlaceBlocks(mousePos, inventory, blocks, &blockCount);
                    DrawTexture(grassTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
                    break;
                case 3:
                    PlaceBlocks(mousePos, inventory, blocks, &blockCount);
                    DrawTexture(waterTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
                    break;
                case 4:
                    //DrawTexture(grassTex, mousePos.x, mousePos.y, TRANSPARENT_WHITE);
                    RemoveBlock(mousePos, blocks, &blockCount);
                    DrawRectangle(mousePos.x, mousePos.y,10,10,ERASER_PINK);
                    break;
            }
        }


        EndMode2D();

        // Draw Inventory UI
        if (editMode) DrawInventory(inventory);

        // Draw FPS and other info
        DrawFPS(10, 10);
        DrawText(TextFormat("Player position: %.2f, %.2f", player.rect.x, player.rect.y), DB_X, DB_Y + 25, DB_F, DB_CL);
        DrawText(TextFormat("Block count: %d", blockCount), DB_X, DB_Y + 50, DB_F, DB_CL);
        DrawText(TextFormat("Active slot: %d", activeSlot), DB_X, DB_Y + 75, DB_F, DB_CL);

        EndDrawing();
    }
    //SaveMap(blocks,blockCount);
    // Clean up textures
    for (int i = 0;i<INVENTORY_SIZE;i++){
        UnloadTexture(inventory[i].tex);
    }

    CloseWindow(); // Close the window and OpenGL context

    return 0;
}

// Function to move the player
void PlayerControlsWithCollision(Player *player, Block *envItems, int envItemsLength) {
    // Store the original position before movement
    Rectangle originalRect = player->rect;

    // Move based on key input
    if (IsKeyDown(KEY_A)) {
        player->rect.x -= 4; // Move left
    } else if (IsKeyDown(KEY_D)) {
        player->rect.x += 4; // Move right
    }

    if (IsKeyDown(KEY_W)) {
        player->rect.y -= 4; // Move up
    } else if (IsKeyDown(KEY_S)) {
        player->rect.y += 4; // Move down
    }

    // Check for collisions after movement
    checkCollisions(envItems, envItemsLength, player);

    // If collision happened, reset position to original
    if (CheckCollisionRecs(player->rect, envItems[0].rect)) {
        player->rect = originalRect; // Revert to original position if collision detected
    }
}

// Function to move the player
void PlayerControls(Player *player) {
    if (IsKeyDown(KEY_A)) {
        player->rect.x -= 4;
    } else if (IsKeyDown(KEY_D)) {
        player->rect.x += 4;
    }

    if (IsKeyDown(KEY_W)) {
        player->rect.y -= 4;
    } else if (IsKeyDown(KEY_S)) {
        player->rect.y += 4;
    }
}

// Function to update the camera center based on player position
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height) {
    camera->offset = (Vector2){ width / 2.0f, height / 2.0f };
    camera->target = (Vector2){ player->rect.x, player->rect.y };
}

// Function to update the active inventory slot
void UpdateActiveSlot() {
    if (IsKeyPressed(KEY_ONE)) {
        activeSlot = 0;
    } else if (IsKeyPressed(KEY_TWO)) {
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

// Function to draw the inventory UI
void DrawInventory(InventorySlot *inventory) {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        // Highlight the active slot with a border
        Color slotColor = (i == activeSlot) ? GOLD : (Color){ 128, 128, 128, 255 };

        // Draw the inventory slot with a background color
        DrawRectangleRec(inventory[i].rect, slotColor);

        DrawTextureEx(inventory[i].tex, (Vector2){inventory[i].rect.x + 20, inventory[i].rect.y + 20}, 0,4,WHITE);
    }
}

// Function to place blocks (sand or dirt) based on the active inventory slot
void PlaceBlocks(Vector2 mousePos, InventorySlot *inventory, Block *blocks, int *blockCount) {
    if (IsMouseButtonDown(0) && *blockCount < MAX_BLOCKS) {
        int canPlace = 1;
        
        // Check if the spot is already occupied
        for (int i = 0; i < *blockCount; i++) {
            Vector2 envPos = (Vector2){ blocks[i].rect.x, blocks[i].rect.y };
            if (Vector2Equals(envPos, mousePos)) {
                canPlace = 0;
                break;
            }
        }

        if (canPlace) {
            // Place the block depending on the active slot (sand or dirt)
            Texture2D currentTex = inventory[activeSlot].tex;
            Block newBlock = { 
                (Rectangle){ mousePos.x, mousePos.y, 10, 10 },
                currentTex 
            };
            blocks[*blockCount] = newBlock;
            (*blockCount)++;
        }
    }
}

void RemoveBlock(Vector2 mousePos, Block *blocks, int *blockCount)
{
    if (IsMouseButtonDown(0)){
        for (int i = 0; i < *blockCount; i++) {
            if (CheckCollisionPointRec(mousePos, blocks[i].rect)) {
                // Shift blocks down in the array
                for (int j = i; j < *blockCount - 1; j++) {
                    blocks[j] = blocks[j + 1];
                }
                (*blockCount)--;  // Decrease the block count
                break;
            }
        }
    }
}
void SaveMap(Block *blocks, int blockCount) {
    FILE *file = fopen("saved_map.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for saving!\n");
        return;
    }

    for (int i = 0; i < blockCount; i++) {
        // Write the block's position and texture index to the file
        fprintf(file, "%f,%f,%d\n", blocks[i].rect.x, blocks[i].rect.y, blocks[i].tex);
    }

    fclose(file);
    printf("Map saved successfully!\n");
}

void checkCollisions( Block *envItems, int envItemsLength, Player *player){
    
        Vector2 centerA, centerB;
        Vector2 subtract;
        Vector2 halfWidthA,halfWidthB;
        float minDistX, minDistY;  
        //topCollision = false;
       
	//int i = NON_BLOCKING_ENVITEMS. Keep background in beginning of array
	for (int i = 5; i < envItemsLength; i++)
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
                        //topCollision = true;
                        //player->speed = 0;
                        //player->rect.y += 1; //prevent flickering
                    }
                    player->rect.y -= copysignf(minDistY, subtract.y); 
                    //player->speed = 0; // bounce off bottom of object
                } 
            }
        
        
        /*
            else{
                topCollision = false;
            }
       */ 
    }
}
