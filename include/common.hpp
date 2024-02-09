#ifndef COMMON_H
#define COMMON_H

// window dimensions for bullet-hell game (東方永夜抄 〜 Imperishable Night)
#define WIDTH 384
#define HEIGHT 384 // actually 448, but dimensions should be square for get_input()

#define INPUT_SIZE 32
#define PLAYER_SIZE 5

#define FRAMES_PER_ACTION 3
#define FRAME_TIME (double)(12 / FRAMES_PER_ACTION) / 60
#define ACTION_TIME (double)12 / 60
#define ACTION_THRESHOLD 0.25

typedef struct vec2_t
{
    float x;
    float y;
} vec2;

typedef struct bullet_t
{
    vec2 pos;
    vec2 size;
} bullet;

// player size is constant
typedef struct player_t
{
    vec2 pos;
} player;

#endif
