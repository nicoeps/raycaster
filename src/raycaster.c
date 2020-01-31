#include "SDL2/SDL.h"

#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCALE 1

int WORLD_MAP[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,2,2,2,2,2,0,0,2,0,3,0,4,0,1},
    {1,0,2,0,0,0,2,0,0,0,0,0,0,0,0,1},
    {1,0,2,0,6,0,2,0,0,5,0,3,0,5,0,1},
    {1,0,2,0,0,0,2,0,0,0,0,0,0,0,0,1},
    {1,0,2,2,0,2,2,0,0,3,0,4,0,2,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,5,5,5,5,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,0,0,5,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,5,0,5,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,5,0,0,0,0,0,0,0,0,4,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} Direction;

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Player {
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;
    float move_speed;
    float rot_speed;
} Player;

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

Color RED = {255, 0, 0};
Color GREEN = {0, 255, 0};
Color BLUE = {0, 0, 255};
Color YELLOW = {255, 255, 0};
Color MAGENTA = {255, 0, 255};
Color CYAN = {0, 255, 255};
Color WHITE = {255, 255, 255};

void move(Player *player, Direction direction) {
    Vector2 dir = player->dir;
    if (direction == BACKWARD) {
        dir.x = -dir.x;
        dir.y = -dir.y;
    }
    if (WORLD_MAP[(int) (player->pos.x + dir.x)][(int) player->pos.y] == 0) {
        player->pos.x += dir.x * player->move_speed;
    }
    if (WORLD_MAP[(int) player->pos.x][(int) (player->pos.y + dir.y)] == 0) {
        player->pos.y += dir.y * player->move_speed;
    }
}

void rotate(Player *player, Direction direction) {
    Vector2 dir = player->dir;
    Vector2 plane = player->plane;
    float rot_speed = player->rot_speed;
    if (direction == RIGHT) {
        rot_speed = -rot_speed;
    }
    player->dir.x = dir.x * cos(rot_speed) - dir.y * sin(rot_speed);
    player->dir.y = dir.x * sin(rot_speed) + dir.y * cos(rot_speed);
    player->plane.x = plane.x * cos(rot_speed) - plane.y * sin(rot_speed);
    player->plane.y = plane.x * sin(rot_speed) + plane.y * cos(rot_speed);
}

void raycast(Player player, SDL_Renderer *renderer) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float camera_x = 2 * x / (float) SCREEN_WIDTH - 1;
        Vector2 ray = {
            player.dir.x + player.plane.x * camera_x,
            player.dir.y + player.plane.y * camera_x
        };

        int map_x = player.pos.x;
        int map_y = player.pos.y;

        Vector2 step;
        Vector2 dist;
        Vector2 delta = {
            fabs(1 / ray.x),
            fabs(1 / ray.y)
        };

        float wall_dist;
        int collision = 0;
        int side;

        if (ray.x < 0) {
            step.x = -1;
            dist.x = (player.pos.x - map_x) * delta.x;
        } else {
            step.x = 1;
            dist.x = (map_x + 1.0 - player.pos.x) * delta.x;
        }
        if (ray.y < 0) {
            step.y = -1;
            dist.y = (player.pos.y - map_y) * delta.y;
        } else {
            step.y = 1;
            dist.y = (map_y + 1.0 - player.pos.y) * delta.y;
        }

        while (collision == 0) {
            if (dist.x < dist.y) {
                dist.x += delta.x;
                map_x += step.x;
                side = 0;
            } else {
                dist.y += delta.y;
                map_y += step.y;
                side = 1;
            }
            if (WORLD_MAP[map_x][map_y] > 0) {
                collision = 1;
            }
        }

        if (side == 0) {
            wall_dist = (map_x - player.pos.x + (1 - step.x) / 2) / ray.x;
        } else {
            wall_dist = (map_y - player.pos.y + (1 - step.y) / 2) / ray.y;
        }

        int line = SCREEN_HEIGHT / wall_dist;
        int start = -line / 2 + SCREEN_HEIGHT / 2;
        if (start < 0) {
            start = 0;
        }
        int end = line / 2 + SCREEN_HEIGHT / 2;
        if (end >= SCREEN_WIDTH) {
            end = SCREEN_HEIGHT - 1;
        }

        Color color;
        switch(WORLD_MAP[map_x][map_y]) {
            case 1:  color = RED;     break;
            case 2:  color = GREEN;   break;
            case 3:  color = BLUE;    break;
            case 4:  color = YELLOW;  break;
            case 5:  color = MAGENTA; break;
            default: color = CYAN;    break;
        }

        if (side == 1) {
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, x, start, x, end);
    }
}

int main() {
    Player player = {
        {14.5, 7.5},
        {-1, 0},
        {0, 0.66},
        1, 0.175
    };

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) == 0) {
            SDL_SetWindowSize(window, SCREEN_WIDTH*SCALE, SCREEN_HEIGHT*SCALE);
            SDL_RenderSetScale(renderer, SCALE, SCALE);

            SDL_Event event;
            SDL_bool done = SDL_FALSE;
            while (!done) {
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                raycast(player, renderer);
                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT:
                            done = SDL_TRUE;
                            break;

                        case SDL_KEYDOWN:
                            switch (event.key.keysym.sym) {
                                case SDLK_UP:    move(&player, FORWARD);  break;
                                case SDLK_DOWN:  move(&player, BACKWARD); break;
                                case SDLK_LEFT:  rotate(&player, LEFT);   break;
                                case SDLK_RIGHT: rotate(&player, RIGHT);  break;
                            }
                            break;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}
