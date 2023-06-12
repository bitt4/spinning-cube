#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

struct Point2d {
    float x;
    float y;
};

struct Point3d {
    float x;
    float y;
    float z; // this is depth in this case, not height like in maths
};

struct Point3d rotate_y(struct Point3d p, float angle) {
    struct Point3d rotated = {
        p.x * cosf(angle) + p.z * sinf(angle),
        p.y,
        p.x * sinf(angle) - p.z * cosf(angle),
    };

    return rotated;
}

struct Point2d project_to_plane(struct Point3d p0) {
    struct Point2d p = {
        p0.x / p0.z,
        p0.y / p0.z,
    };

    return p;
}

struct Point2d world_to_screen_coords(struct Point2d p,
                                      const int screen_width,
                                      const int screen_height) {
    p.x = (p.x + 1) * (screen_width / 2);
    p.y = (p.y + 1) * (screen_height / 2);

    return p;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL Initialization error: %s\n", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "SDL_Init",
                                 SDL_GetError(),
                                 0);
        return 1;
    }

    const int WINDOW_HEIGHT = 800;
    const int WINDOW_WIDTH = 800;

    SDL_Window* window = SDL_CreateWindow("Spinning Cube",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        SDL_Log("Window creation failed: %s\n", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "SDL_CreateWindow",
                                 SDL_GetError(),
                                 0);
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_Log("Renderer creation failed: %s\n", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "SDL_CreateRenderer",
                                 SDL_GetError(),
                                 0);
        return 1;
    }

    struct Point3d cube[8] = {
        {-1.0f, -1.0f, -1.0f}, //     .8------7
        { 1.0f, -1.0f, -1.0f}, //   .' |    .'|
        { 1.0f, -1.0f,  1.0f}, //  5---+--6'  |
        {-1.0f, -1.0f,  1.0f}, //  |   |  |   |
        {-1.0f,  1.0f, -1.0f}, //  |  ,4--+---3
        { 1.0f,  1.0f, -1.0f}, //  |.'    | .'
        { 1.0f,  1.0f,  1.0f}, //  1------2'
        {-1.0f,  1.0f,  1.0f}, //
    };

    float angle = 0.0f;

    SDL_Event e;

    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0);

        struct Point2d r[8]; // rotated cube on screen
        for (size_t i = 0; i < sizeof(cube) / sizeof(cube[0]); ++i) {
            struct Point3d rotated_point = rotate_y(cube[i], angle);

            // put the point further from the camera
            rotated_point.z += 3.0f;

            r[i] = world_to_screen_coords(
                project_to_plane(rotated_point),
                WINDOW_WIDTH,
                WINDOW_HEIGHT);
        }

        // draw edges of the cube
        // bottom edges
        for (int i = 0; i < 4; ++i) {                               // 1 -> 2
            SDL_RenderDrawLine(renderer,                            // 2 -> 3
                               r[i].x, r[i].y,                      // 3 -> 4
                               r[(i + 1) % 4].x, r[(i + 1) % 4].y); // 4 -> 1
        }

        // edges that connect bottom and top side
        for (int i = 0; i < 4; ++i) {                   // 1 -> 5
            SDL_RenderDrawLine(renderer,                // 2 -> 6
                               r[i].x, r[i].y,          // 3 -> 7
                               r[i + 4].x, r[i + 4].y); // 4 -> 8
        }

        // top edges
        for (int i = 0; i < 4; ++i) {                                       // 5 -> 6
            SDL_RenderDrawLine(renderer,                                    // 6 -> 7
                               r[i + 4].x, r[i + 4].y,                      // 7 -> 8
                               r[(i + 1) % 4 + 4].x, r[(i + 1) % 4 + 4].y); // 8 -> 5
        }

        SDL_RenderPresent(renderer);

        const int FPS = 60;
        SDL_Delay(1000 / FPS);

        const float ROTATION_SPEED = 3.0f;
        float dt = 1.0f / FPS;
        angle += ROTATION_SPEED * dt;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
