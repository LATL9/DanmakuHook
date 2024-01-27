typedef struct vec2_t
{
    long x;
    long y;
} vec2;

typedef struct bullet_t
{
    vec2 pos;
    vec2 size;
} bullet;

// player size is fixed
typedef struct player_t
{
    vec2 pos;
} player;
