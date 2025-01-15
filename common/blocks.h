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
void TransparentCursorColor(Camera2D *camera,Color color);
void PlaceBlock(Rectangle *blocks,int *blockCount,int blockLen, Texture2D *tex, Vector2 mousePos);
void UpdateActiveSlot(int *activeSlot);
void RemoveBlock(Rectangle *blocks, int *blockCount, Vector2 mousePos);
//for grass, dirt and water arrays
void RemoveBlock_3(Rectangle *b1, int *bc1,
                   Rectangle *b2, int *bc2,
                   Rectangle *b3, int *bc3,
                   Vector2 mousePos);
