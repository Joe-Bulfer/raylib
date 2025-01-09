/*
gcc better-envItems.c -o better-envItems -I ../../projects/raylib-quickstart/build/external/raylib-master/src/ -L ../../projects/raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./better-envItems
*/
//will become common.c or h
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define ra radius
#define p position
#define r rect
#define h height
#define w width
#define c color

typedef struct EnvItem {
    Color color;
    enum { CIRCLE,TRIANGLE,RECTANGLE,POLYGON } type;
    union {
        struct { //circle
            int radius;
            Vector2 position;
        };
        struct {//triangle
            Vector2 t_points[3];
        };
        //rectangle from raylib.h
        Rectangle rect;
        struct {//polygon
            int num_points;
            Vector2 *points;
        };
    };
} EnvItem;


#define MAX_R 4
#define MAX_C 4
#define MAX_T 4
//most of the world_r will be made of rects
EnvItem world_r[MAX_R] = {
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){50, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){250, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){450, 200, 100, 150}
    },
    {
        .type = RECTANGLE,
        .color = BLUE,
        .rect = (Rectangle){650, 200, 100, 150}
    },
};
EnvItem world_c[MAX_C] = {
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){50, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){150, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){250, 100}
    },
    { // Circle
        .type = CIRCLE,
        .color = YELLOW,
        .radius = 50,
        .position = (Vector2){350, 100}
    },
};

EnvItem world_t[MAX_T] = {
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{0, 0}, {50, 50}, {100, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = GREEN,
        //.t_points = {{100, 100}, {150, 150}, {100, 100}}
        .t_points = {{100, 0}, {150, 50}, {200, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{200, 0}, {250, 50}, {300, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{300, 0}, {350, 50}, {400, 0}}
    },
    { // Triangle
        .type = TRIANGLE,
        .color = PINK,
        .t_points = {{400, 300}, {450, 450}, {400, 440}}
    },
};

void DrawWorld(){
    for(int i=0;i<MAX_R;i++){
        DrawRectangle(world_r[i].r.x,world_r[i].r.y,world_r[i].r.w,world_r[i].r.h,world_r[i].c);
    }
    for(int i=0;i<MAX_C;i++){
        DrawCircleV(world_c[i].p,world_c[i].ra,world_c[i].c);
    }
    
    for(int i=0;i<MAX_T;i++){
        DrawTriangle(world_t[i].t_points[0],world_t[i].t_points[1],world_t[i].t_points[2],world_t[i].c);
    }

}

/*
 * when i add this simple main function and comment out the main function the bottom, I get this error
 
```c
int main(){
    return 0;
}
```
 
```terminal
raylib-quickstart/bin/Debug/ -lraylib -lm -lpthread -ldl -lX11 && ./better-envItems
better-envItems.c:112:5: warning: excess elements in array initializer
  112 |     { // Triangle
      |     ^
better-envItems.c:112:5: note: (near initialization for ‘world_t’)
```

All I want is to simplify this c file to eventually use it as a sort of header or definition file which I can call like this:

```c
#include "world.c"

//... more stuff
int main(){
..raylib stuff
DrawWorld();
..more raylib stuff
}
```

*/


int main(){
    int screenWidth, screenHeight;
    screenWidth = 800; screenHeight = 500;
    InitWindow(screenWidth,screenHeight,"BoilerPlate");
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RED);

        DrawWorld();

        EndDrawing();
    }
    CloseWindow();
}

