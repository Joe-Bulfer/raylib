#include "raylib.h"
#include "world.h"
#include "player-projectile.h"
#define PL_RADIUS 30 //radius

void UpdateCameraCenter(Camera2D *camera, Player *player, int width, int height) {
    camera->offset = (Vector2){ width / 2.0f, height / 2.0f };
    camera->target = (Vector2){ player->pos.x, player->pos.y };
}
void CameraZoom(Camera2D *camera, float min, float max){
    camera->zoom += ((float)GetMouseWheelMove() * 0.15f);

    if (camera->zoom > max) camera->zoom = max;
    else if (camera->zoom < min) camera->zoom = min;
}

void PlayerControls(Player *player, EnvItem *blocks)
{
    Vector2 originalPos = player->pos;

    if (IsKeyDown(KEY_A)) {
        player->pos.x -= PLAYER_SPEED;
        if (IsCollision(player, blocks)) {
            player->pos.x = originalPos.x; // Reset X if collision occurs
        }
    }
    if (IsKeyDown(KEY_D)) {
        player->pos.x += PLAYER_SPEED;
        if (IsCollision(player, blocks)) {
            player->pos.x = originalPos.x; // Reset X if collision occurs
        }
    }
    if (IsKeyDown(KEY_W)) {
        player->pos.y -= PLAYER_SPEED;
        if (IsCollision(player, blocks)) {
            player->pos.y = originalPos.y; // Reset Y if collision occurs
        }
    }
    if (IsKeyDown(KEY_S)) {
        player->pos.y += PLAYER_SPEED;
        if (IsCollision(player, blocks)) {
            player->pos.y = originalPos.y; // Reset Y if collision occurs
        }
    }
}

// only used within PlayerControls on each key WASD
bool IsCollision(Player *player, EnvItem *blocks)
{
    for (int i = 0; i < MAX_R; i++) {
        if (CheckCollisionCircleRec(player->pos, PL_RADIUS-2, blocks[i].rect)) {
            return true; // Collision detected
        }
    }
    return false; // No collision detected
}
