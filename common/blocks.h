#include "raylib.h"

typedef struct EnvItem EnvItem;
/*
typedef struct Vector2 Vector2;
typedef struct Camera2D Camera2D ;
*/

extern Vector2 mousePos;
extern int areaFree;

void CameraDrag(Camera2D *camera);
void TransparentCursorTex(Camera2D *camera,Texture2D tex);
void TransparentCursor(Camera2D *camera);
void PlaceBlock(Rectangle *blocks,int *blockCount,int blockLen, Texture2D *tex, Vector2 mousePos);
void UpdateActiveSlot(int *activeSlot);

