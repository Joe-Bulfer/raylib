/*
gcc knapsack.c -o knapsack -I ../raylib-quickstart/build/external/raylib-master/src/ -L ../raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11
*/

//now you can pick up sticks. Next integrate existing place block feature to place sticks
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include <stdlib.h> // Required for: copysignf()
// debug x, y, font, color
#define DB_X 1090
#define DB_Y 20
#define DB_F 20
#define DB_CL                 (Color){ 155, 21, 41, 255 }
#define TRANSPARENT_BLUE      (Color){ 0, 121, 241, 55 } //for cursor
#define INVENTORY_GRAY      (Color){ 150, 121, 241, 85 } 
#define G 500 
#define FPS 30 
#define PLAYER_START_X 330
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
    int weight ;
    int value ;
    Color color;
    bool collected;
} Item;

//default stick color 
#define BIRCH                 (Color){ 180, 180, 180, 255 }
#define OAK                   (Color){ 100, 50, 0, 255 }
#define SPRUCE                (Color){ 100, 150, 150, 255 }

#define MAX_STICKS 9
#define STACK_SIZE 64
//Item sticksArray[MAX_STICKS]; 
Item collectedSticksArray[MAX_STICKS];
Item sticksArray[MAX_STICKS] = {
    {{-30, 397, 20, 10},3, 1, BIRCH, false},
    {{-80, 397, 20, 10},3, 1, BIRCH, false},
    {{50, 397, 20, 10},3, 1, BIRCH, false},

    {{99, 397, 20, 10}, 2, 1, OAK, false},
    {{78, 397, 20, 10}, 2, 1, OAK, false},

    {{610, 397, 20, 10},3, 1, SPRUCE, false},
    {{570, 397, 20, 10},3, 1, SPRUCE, false},
    {{650, 397, 20, 10},3, 1, SPRUCE, false},

};

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

//void checkCollisions(EnvItem *envItems, int envItemsLength, Player *player);
void checkCollisions(void *items, int envItemsLength, Player *player, size_t itemSize);

//void placeBlocks(EnvItem *envItems, int envItemsLength);
void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height);

bool topCollision = false;

bool showDebugInfo = true;

bool showInventory = true;

bool placeBlockMode = true;

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "fucking awesome cat game");

    Player player = { 0 };
    player.rect = (Rectangle){ PLAYER_START_X, PLAYER_START_Y,30,30 }; //start position
    Vector2 position; //rect.x,rect.y
    player.speed = 0;
    player.canJump = false;

    //Texture2D backgroundTex = LoadTexture("tree background.png");      
    //the moving background didn't work nice. But I'll keep it's size.
    //float backgroundTex_xpos = -2700; //moves when player moves
    //backgroundTex.width+=4900;
    //backgroundTex.height+=1600;

    Texture2D sandBlock = LoadTexture("rocky-sand-10x10.png");        
    Texture2D playerTex = LoadTexture("cat_run.png");        
    Vector2 mousePos = {0,0}; //mouse hovers over chest
    int canPlace = 0;
    int chestOpen = 0;//click chest
    int withinChestRange = 0;//click chest
    Texture2D chestTex = LoadTexture("chest.png");        
    Rectangle chestMouseRect = {365,360,30,20};
    Rectangle chestRect = {320,330,130,90};
    playerTex.width += 30;
    playerTex.height += 15;
    //Rectangle frameRec = { 0.0f, 0.0f, (float)playerTex.width/4, (float)playerTex.height };
    Rectangle frameRec = { 0.0f, 0.0f, playerTex.width/4, playerTex.height };
    int currentFrame;
    float framesSpeed = 7; // start at minimum walking speed
    int framesCounter; //start at first frame 
    bool moving; //walking left or right with A or D
    int MAX_BLOCKS = 100;
    int blockCount = 4;
    Block *B_TX_Blocks = (Block*)malloc(sizeof(Block) * MAX_BLOCKS  );
    B_TX_Blocks[0] = (Block){{  700, 400, 10, 10 }, sandBlock  }; //floor
    B_TX_Blocks[1] = (Block){{ 700, 410, 10, 10}, sandBlock   }; //floor
    B_TX_Blocks[2] = (Block){{ 680, 410, 10, 10}, sandBlock   }; //floor
    B_TX_Blocks[3] = (Block){{ 710, 410, 10, 10}, sandBlock   }; //floor
    //#define B_ENVITEMS 21
    //#define NB_ENVITEMS 9
    int B_ENVITEMS = 21;
    EnvItem *bEnvItems = (EnvItem*)malloc(sizeof(EnvItem) * B_ENVITEMS );
    //EnvItem envItems[] = {
    int NB_ENVITEMS = 10;
    EnvItem nbEnvItems[] = {
 	    // background
            {{ -1000, -100, 3000, 500 }, SKYBLUE }, //background
            {{ 330, 329, 20,88 }, DARKGRAY }, //piller
            {{ 420, 329, 20,88 }, DARKGRAY }, //piller
        //
            // trees
            {{ 90, 320, 25, 80 }, OAK },
            {{ 80, 300, 45, 80 }, GREEN },

            {{ -90, 320, 25, 80 }, BIRCH },
            {{ -100, 300, 45, 80 }, DARKGREEN },

            {{ 546, 320, 10, 80 }, SPRUCE },
            {{ 533, 300, 35, 80 }, DARKGREEN },
            //water
            {{ 700, 400, 335, 200 }, BLUE },
        };
    // NB_ENVITEMS ENDS
    bEnvItems[0] = (EnvItem){{ -200, 400, 900, 200 }, DARKGREEN }; //floor
    bEnvItems[1] = (EnvItem){{ -750, 400, 300, 900 }, DARKGREEN  }; //floor
    bEnvItems[2] = (EnvItem){{ -450, 800, 300, 500 }, DARKBROWN  }; //floor
    //
// 3 pixel staggered vertically to climb steps
    bEnvItems[3] = (EnvItem){{ 260, 397, 90,10 }, GRAY }; //steps
    bEnvItems[4] = (EnvItem){{ 270, 394, 80,10 }, GRAY }; //steps
    bEnvItems[5] = (EnvItem){{ 280, 391, 70,10 }, GRAY }; //steps
    bEnvItems[6] = (EnvItem){{ 290, 388, 60,10 }, GRAY }; //steps
    bEnvItems[7] = (EnvItem){{ 300, 385, 50,10 }, GRAY }; //steps
    bEnvItems[8] = (EnvItem){{ 310, 382, 40,10 }, GRAY }; //steps
    bEnvItems[9] = (EnvItem){{ 320, 379, 30,10 }, GRAY }; //steps
//
    bEnvItems[10] = (EnvItem){{ 320, 379, 100,28 }, GRAY }; //foundation
    bEnvItems[11] = (EnvItem){{ 320, 310, 130,20 }, GRAY }; //roof
    
//
    bEnvItems[12] = (EnvItem){{ 420, 397, 90,10 }, GRAY }; //steps
    bEnvItems[13] = (EnvItem){{ 420, 394, 80,10 }, GRAY }; //steps
    bEnvItems[14] = (EnvItem){{ 420, 391, 70,10 }, GRAY }; //steps
    bEnvItems[15] = (EnvItem){{ 420, 388, 60,10 }, GRAY }; //steps
    bEnvItems[16] = (EnvItem){{ 420, 385, 50,10 }, GRAY }; //steps
    bEnvItems[17] = (EnvItem){{ 420, 382, 40,10 }, GRAY }; //steps
    bEnvItems[18] = (EnvItem){{ 420, 379, 30,10 }, GRAY }; //steps
    
    //
    bEnvItems[19] = (EnvItem){{ 300, 200, 400, 10 }, DARKBROWN }; //platforms
    //bEnvItems[xx] = (EnvItem){{ 250, 300, 100, 10 }, DARKBROWN };
    bEnvItems[20] = (EnvItem){{ 650, 300, 100, 10 }, DARKBROWN };


    int envItemsLength = sizeof(bEnvItems)/sizeof(bEnvItems[0]);


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
        checkCollisions(bEnvItems, B_ENVITEMS, &player, sizeof(EnvItem));
        checkCollisions(B_TX_Blocks, MAX_BLOCKS, &player, sizeof(Block));


        
        // reset
        if (IsKeyPressed(KEY_R))
        {
                player.rect.x = PLAYER_START_X;
                player.rect.y = PLAYER_START_Y;
                player.speed = 0;
                player.speed = 0;
                camera.zoom = 3.0;
        }

        //frameRec.height = -(float)playerTex.height / 4;  // Negate to flip left
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

        if (IsKeyPressed(KEY_L)) showDebugInfo = !showDebugInfo ;
        if (IsKeyPressed(KEY_I)) showInventory = !showInventory ;
        if (IsKeyPressed(KEY_B)) placeBlockMode = !placeBlockMode  ;
        // Draw
        //-----------------------------------------------------
        UpdateCameraCenter(&camera, &player, screenWidth, screenHeight);


        BeginDrawing();

	BeginMode2D(camera);
        //DrawTexture(backgroundTex,backgroundTex_xpos,-1000,WHITE);


            ClearBackground(BLACK);

            // environment
            for (int i = 0; i < NB_ENVITEMS; i++) DrawRectangleRec(nbEnvItems[i].rect, nbEnvItems[i].color);
            for (int i = 0; i < B_ENVITEMS; i++) DrawRectangleRec(bEnvItems[i].rect, bEnvItems[i].color);
            for (int i = 0; i < blockCount; i++) DrawTexture(B_TX_Blocks[i].tex, B_TX_Blocks[i].rect.x,B_TX_Blocks[i].rect.y,WHITE);

            //sand block test
            //DrawTexture(sandBlock, 365, 350, WHITE);
            mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
            //round to nice even number
            mousePos.x = floorf(mousePos.x / 10) * 10.0f;
            mousePos.y = floorf(mousePos.y / 10) * 10.0f;

            //transparent cursor
            DrawRectangle(mousePos.x, mousePos.y, 10, 10, TRANSPARENT_BLUE);

            if(IsMouseButtonDown(0) && blockCount < MAX_BLOCKS){

                canPlace = 1;
                for (int i = 0; i < blockCount; i++) {
                        Vector2 envPos = (Vector2){B_TX_Blocks[i].rect.x, B_TX_Blocks[i].rect.y};
                        if (Vector2Equals(envPos, mousePos)) {
                                canPlace = 0;
                        }
                }
		if(canPlace){
                    Block newBlock = { 
                        (Rectangle){mousePos.x, mousePos.y, 10, 10},  // Position
                        sandBlock  };
            // Reallocate memory to add the new block
            blockCount++;  // Increase the length
            B_TX_Blocks = (Block*)realloc(B_TX_Blocks, sizeof(B_TX_Blocks) * blockCount);

            // Append the new block
            B_TX_Blocks[blockCount - 1] = newBlock;
            }
}

            // sticksArray in world before collected
	     for (int i = 0; i < MAX_STICKS; i++){
		    if (CheckCollisionRecs(sticksArray[i].rect, player.rect) && sticksArray[i].collected == false){
                            DrawText("E to Collect",
                                sticksArray[i].rect.x,
                                sticksArray[i].rect.y-30,
                                     10,BLACK
                                     );
                            if (IsKeyPressed(KEY_E)) sticksArray[i].collected = true;
			    //sticksArray[i].collected = true;
		    };
		    if (!sticksArray[i].collected){
			    DrawRectangleRec(sticksArray[i].rect, sticksArray[i].color);
			    DrawText(TextFormat("%d, %d",sticksArray[i].value, sticksArray[i].weight),sticksArray[i].rect.x +2,sticksArray[i].rect.y,10,BLACK);
	     }
	    }
            // player
            if (CheckCollisionRecs(player.rect,chestRect)) withinChestRange = 1;
            else {
                withinChestRange = 0;
                chestOpen = 0;
            }
	    mousePos  = GetScreenToWorld2D(GetMousePosition(), camera);
            //see if mouse is inside/on chest
            if(CheckCollisionPointRec(mousePos,chestMouseRect)){ 
                DrawTexture(chestTex, 365, 350, WHITE);
                //DrawRectangleRec(chestRect, PINK);
                if (IsMouseButtonDown(0)&& withinChestRange) chestOpen = 1;
            }else if(chestOpen ==1){
                DrawTexture(chestTex, 365, 350, WHITE);
            }
            else{
                DrawTexture(chestTex, 365, 350, GRAY);                      
            }
            DrawRectangleGradientV(-450,400,250,400,BLACK,DARKBROWN);
            DrawCircle(-300,750,40,GRAY); //boulder
            
            DrawTextureRec(playerTex, frameRec, position, WHITE);  //  how do I scale this?
            //DrawRectangleRec(player.rect, BLUE); //old thing before animation
            

	EndMode2D();
        DrawFPS(10, 10);
        if (showDebugInfo){
            DrawText(TextFormat("player position : %.2f, %.2f",player.rect.x,player.rect.y),DB_X,DB_Y+25,DB_F,DB_CL);
            //DrawText(TextFormat("player speed : %.2f",player.speed),DB_X,DB_Y+50,DB_F,DB_CL);
            DrawText(TextFormat("mousePos : %.2f,%.2f",mousePos.x,mousePos.y),DB_X,DB_Y+50,DB_F,DB_CL);
            if (topCollision)DrawText(TextFormat("topCollision - true"),DB_X,DB_Y+75,DB_F,DB_CL);
            if (player.canJump)DrawText(TextFormat("canJump - true"),DB_X,DB_Y+100,DB_F,DB_CL);
            DrawText(TextFormat("Animation"),DB_X,DB_Y+125,DB_F,DB_CL);
            //DrawText(TextFormat("framesCount - %.2f",framesCount),DB_X,DB_Y+25,DB_CL);
            DrawText(TextFormat("currentFrame - %.2f",currentFrame),DB_X,DB_Y+150,DB_F,DB_CL);
            //DrawText(TextFormat("frameSpeed - %.2f",frameSpeed),DB_X,DB_Y+25,DB_CL);
            DrawText(TextFormat("placeBlockMode  - %d",placeBlockMode  ),DB_X,DB_Y+175,DB_F,DB_CL);
            DrawText(TextFormat("chestOpen: %d\nwithinChestRange: %d",chestOpen,withinChestRange),DB_X,DB_Y+200,DB_F,DB_CL);
            DrawText(TextFormat("MAX_BLOCKS: %d\nblockCount: %d",MAX_BLOCKS,blockCount),DB_X,DB_Y+250,DB_F,DB_CL);
        }
        // inventory
        if (IsKeyPressed(KEY_C)) chestOpen = 0;

        // chest inventory menu
        if (chestOpen && withinChestRange){
            DrawRectangle(screenWidth/2,screenHeight/2+200,400,200,INVENTORY_GRAY);
            DrawLine(screenWidth/2+100,screenHeight/2+200,
                     screenWidth/2+100, screenHeight/2+400, BLACK);
            DrawLine(screenWidth/2+200,screenHeight/2+200,
                     screenWidth/2+200, screenHeight/2+400, BLACK);
            DrawLine(screenWidth/2+300,screenHeight/2+200,
                     screenWidth/2+300, screenHeight/2+400, BLACK);
            DrawLine(screenWidth/2,screenHeight/2+300,
                     screenWidth/2+400, screenHeight/2+300, BLACK);
        }
	
        if (showInventory){
	 for (int i = 0; i < MAX_STICKS; i++){
	     	if (sticksArray[i].collected){
                    DrawRectangle(50+(i*40),40,40,40,INVENTORY_GRAY);
		    sticksArray[i].rect.x = 50 +(i*40);
		    sticksArray[i].rect.y = 50;
		    DrawRectangleRec(sticksArray[i].rect,sticksArray[i].color);
		    //stickCount++;
            }
	 }
        }
        EndDrawing();
        //-----------------------------------------------------
    }
    //UnloadTexture(backgroundTex);
    UnloadTexture(playerTex);
    UnloadTexture(sandBlock);
    CloseWindow();        // Close window and OpenGL context
    return 0;
}

//function before adding polymorphic behavior
//void checkCollisions(Rectangle *boxA, Player *player){
/*
void checkCollisions( EnvItem *envItems, int envItemsLength, Player *player){
    
        Vector2 centerA, centerB;
        Vector2 subtract;
        Vector2 halfWidthA,halfWidthB;
        float minDistX, minDistY;  
        topCollision = false;
       
	// non blocking (NB) envItems to keep background it start of array
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
    }
}
*/
void checkCollisions(void *items, int envItemsLength, Player *player, size_t itemSize) {
    //"void *items" is to act polymorphic and accept either
    //EnvItem or Block.
    //it only modifies the rectangle, which both have. 
    //Only thing different between EnvItem and Block is
    //envItem has only color, Block has texture
    //BEGIN wierd polymorphic stuff
    for (int i = 0; i < envItemsLength; i++) {
        // Get pointer to the i-th item
        void *item = (char *)items + i * itemSize;

        // Access the `rect` field of the current item
        Rectangle *rect = NULL;
        if (itemSize == sizeof(EnvItem)) {
            rect = &((EnvItem *)item)->rect;  // If it's an EnvItem
        } else if (itemSize == sizeof(Block)) {
            rect = &((Block *)item)->rect;    // If it's a Block
        }
    //END wierd polymorphic stuff

        if (rect && CheckCollisionRecs(*rect, player->rect)) {
            // Handle the collision as before
            Vector2 centerA = (Vector2){ rect->x + rect->width / 2, rect->y + rect->height / 2 };
            Vector2 centerB = (Vector2){ player->rect.x + player->rect.width / 2, player->rect.y + player->rect.height / 2 }; 
            Vector2 subtract = Vector2Subtract(centerA, centerB);
            Vector2 halfWidthA = (Vector2){ rect->width * 0.5f, rect->height * 0.5f };
            Vector2 halfWidthB = (Vector2){ player->rect.width * 0.5f, player->rect.height * 0.5f };
            float minDistX = halfWidthA.x + halfWidthB.x - fabsf(subtract.x);
            float minDistY = halfWidthA.y + halfWidthB.y - fabsf(subtract.y);

            if (minDistX < minDistY) {
                // Collide left or right
                player->rect.x -= copysignf(minDistX, subtract.x);
            } else {
                // Collide top or bottom
                if (subtract.y > 0) {
                    topCollision = true;
                    player->speed = 0;
                }
                player->rect.y -= copysignf(minDistY, subtract.y); 
                player->speed = 0; // bounce off the bottom
            }
        }
    }
}

void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height){
	camera->offset = (Vector2){ width/2.0f, height/2.0f };
	camera->target = (Vector2){player->rect.x,player->rect.y};

}
