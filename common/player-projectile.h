/*
* could copy functions from grenade.c
* want to include bother overhead-shooter/grenade and gravity based 2d platformer functions
* or could seperate that into ...
*/
#include <math.h> //for fabs
#include "raymath.h"


//#define PI 3.14159 //in raylib already?
#define PLAYER_SPEED 2.5 //for moving with WASD in PlayerControls
//
typedef struct Player {
    Vector2 pos; //position
    Color color;
} Player;

//projectile begin
extern float grenadeSpeedX;
extern float grenadeSpeedY;
extern Vector2 grenadePos;  // starts at player position
#define GRENADE_RADIUS 5
extern Vector2 mousePos;
extern Vector2 centerV;//center of screen
//angle from center to mouse position determines projectile speedX and Y with cos and sin respectively

extern float angleRad;
extern float angleDeg;
//projectile end

//in c, all functions are implicit extern unless explicitly static
//structs don't need extern but instances do

void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height);
void CameraZoom(Camera2D *camera, float max, float min);
void PlayerControls(Player *player, EnvItem *blocks);
bool IsCollision(Player *player, EnvItem *blocks); //only used within PlsyerControls
void DebugText(Player player, Camera camera,double elapsedTime,float angleDeg);
