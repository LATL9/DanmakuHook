typedef struct vec2_t
{
    float x;
    float y;
} vec2;

typedef struct entity_t
{
    vec2 pos;
    vec2 size;
} entity

typedef struct bullet_t : entity_t { } bullet
typedef struct player_t : entity_t { } player
