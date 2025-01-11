#include "raylib.h"

typedef struct EnvItem EnvItem;
/*
typedef struct Vector2 Vector2;
typedef struct Camera2D Camera2D ;
*/

extern Vector2 mousePos;
extern int areaFree;

void CameraDrag(Camera2D *camera);
void TransparentCursorTexture(Camera2D *camera,Texture2D text);
void TransparentCursor(Camera2D *camera);
void PlaceBlock(EnvItem **blocks,int blockCount,int blockLen);

