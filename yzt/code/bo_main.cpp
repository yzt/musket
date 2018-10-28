#include <sdl2/SDL.h>
#include <cstdio>
//#include <thread>

using byte = unsigned char;

#include "bo_math.hpp"
#include "bo_render.hpp"

int main (int argc, char * argv []) {
    float state_movement = 0.0f;  // in [-1..1]
    bool state_ball_moving = false;
    Point state_paddle_pos = {};
    Point state_ball_pos = {};
    Vec2 state_ball_dir = {};
    bool input_left = false;
    bool input_right = false;
    bool input_action = false;
    bool input_exit = false;

    float config_paddle_default_speed = 1000.0f;
    int config_paddle_default_width = 150;
    int config_paddle_default_height = 30;
    float config_paddle_default_y = 0.90f;
    float config_ball_default_radius = 10.0f;
    float config_ball_default_speed = 30.0f;
    int config_target_fps = 3;
    int config_window_height = 960;
    float config_aspect_ratio = 3.0f / 4.0f;   //9.0f / 16;

    state_paddle_pos.y = config_paddle_default_y * config_window_height;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_CreateWindowAndRenderer(int(config_window_height * config_aspect_ratio + 0.5f), config_window_height, 0 /*| SDL_WINDOW_FULLSCREEN*/, &window, &renderer);
    SDL_SetWindowTitle(window, "BrykOut");

    int window_width = 0, window_height = 0;
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_Texture * tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    SDL_assert(tex);

    double target_frame_time_s = 1.0 / config_target_fps;
    double const inv_pfc_freq = 1.0 / SDL_GetPerformanceFrequency();
    double next_frame_start_s = inv_pfc_freq * SDL_GetPerformanceCounter() + target_frame_time_s;

    SDL_Event ev = {};
    unsigned t0 = SDL_GetTicks();
    unsigned frame_count = 0;
    double wastage = 0;
    while (!input_exit) {
        // Process pending events...
        input_action = false;
        input_exit = false;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                case SDLK_SPACE: input_action = true; break;
                case SDLK_a: case SDLK_LEFT: input_left = true; break;
                case SDLK_d: case SDLK_RIGHT: input_right = true; break;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE: input_exit = true; break;
                case SDLK_a: case SDLK_LEFT: input_left = false; break;
                case SDLK_d: case SDLK_RIGHT: input_right = false; break;
                }
                break;
            case SDL_QUIT:
                input_exit = true;
                break;
            }
        }

        // Process the input...
        if (input_left && !input_right)
            state_movement = -1.0f;
        else if (input_right && !input_left)
            state_movement = 1.0f;
        else
            state_movement = 0.0f;

        if (input_action && !state_ball_moving) {
            state_ball_moving = true;
            state_ball_dir = Normalized({state_movement < 0 ? -1.0f : 1.0f, -1.0f});
        }

        // Do the update...
        state_paddle_pos.x += state_movement * config_paddle_default_speed * float(target_frame_time_s);
        state_paddle_pos.x = Clamp(state_paddle_pos.x, 0, Real(window_width - config_paddle_default_width));

        if (!state_ball_moving) {
            state_ball_pos = state_paddle_pos + Vec2{config_paddle_default_width / 2.0f, -config_ball_default_radius};
        } else {
            auto ball_intended_pos = state_ball_pos + state_ball_dir * (config_ball_default_speed * float(target_frame_time_s));

            // Detect and handle collisions... Method 1!
            Point wall_min = {0, 0};
            Point wall_max = {Real(window_width), Real(window_height)};

            wall_min += config_ball_default_radius;
            wall_max -= config_ball_default_radius;

            if (ball_intended_pos.x < wall_min.x)
                if (state_ball_dir.x < 0)
                    state_ball_dir.x = -state_ball_dir.x;
            if (ball_intended_pos.y < wall_min.y)
                if (state_ball_dir.y < 0)
                    state_ball_dir.y = -state_ball_dir.y;
            if (ball_intended_pos.x > wall_max.x)
                if (state_ball_dir.x > 0)
                    state_ball_dir.x = -state_ball_dir.x;
            if (ball_intended_pos.y > wall_max.y)
                if (state_ball_dir.y > 0)
                    state_ball_dir.y = -state_ball_dir.y;
            
            // Detect and handle collisions... Method 2!
            Real const R = config_ball_default_radius;
            Point const corners [4] = {
                {0 + R, 0 + R},                         // top-left
                {0 + R, window_height - R},             // bottom-left
                {window_width - R, window_height - R},  // bottom-right
                {window_width - R, 0 + R},              // top-right
            };

            for (int i = 0; i < 4; ++i) {
                auto res = Collides_SegmentToSegment(state_ball_pos, ball_intended_pos, corners[i], corners[(i + 1) % 4]);
                if (res.exists && res.ta >= 0 && res.ta <= 1 && res.tb >= 0 && res.tb <= 1)
                    ::printf("\n%d, %s, %8.3f, %8.3f\n", i, res.exists ? "YES" : "NO ", double(res.ta), double(res.tb));
            }

            state_ball_pos = ball_intended_pos;
        }


        // Do the render...
        Canvas canvas = {};
        SDL_LockTexture(tex, nullptr, &canvas.pixels_raw, &canvas.pitch_bytes);
        canvas.width = window_width;
        canvas.height = window_height;

        Render_Clear(&canvas, {0, 0, 0});

    #if 0
        *canvas.pixel(100, 50) = Color{255, 255, 0};
        *canvas.pixel(101, 50) = Color{255, 255, 0};
        *canvas.pixel(102, 50) = Color{255, 255, 0};
        *canvas.pixel(103, 50) = Color{255, 255, 0};
        *canvas.pixel(100, 51) = Color{255, 255, 0};
        *canvas.pixel(101, 51) = Color{255, 255, 0};
        *canvas.pixel(102, 51) = Color{255, 255, 0};
        *canvas.pixel(103, 51) = Color{255, 255, 0};
        *canvas.pixel(100, 52) = Color{255, 255, 0};
        *canvas.pixel(101, 52) = Color{255, 255, 0};
        *canvas.pixel(102, 52) = Color{255, 255, 0};
        *canvas.pixel(103, 52) = Color{255, 255, 0};
        *canvas.pixel(100, 53) = Color{255, 255, 0};
        *canvas.pixel(101, 53) = Color{255, 255, 0};
        *canvas.pixel(102, 53) = Color{255, 255, 0};
        *canvas.pixel(103, 53) = Color{255, 255, 0};

        Render_Circle(&canvas, 200, 200, 50, {0, 255, 0});
        Render_Circle(&canvas, 275, 200, 25, {0, 0, 255});
    #endif

        Render_AAB(
            &canvas,
            Round(state_paddle_pos.x), Round(state_paddle_pos.y),
            config_paddle_default_width, config_paddle_default_height,
            {255, 0, 0}
        );

        Render_Circle(
            &canvas,
            Round(state_ball_pos.x), Round(state_ball_pos.y),
            Round(config_ball_default_radius),
            {255, 255, 255}
        );

        SDL_UnlockTexture(tex);

        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // FPS counter ...
        frame_count += 1;
        unsigned t1 = SDL_GetTicks();
        if (t1 - t0 >= 1 * 1000) {
            ::printf("FPS = %5.1f, frame time = %5.2f ms, wastage = %5.2f ms\r"
                , double(frame_count) / (t1 - t0) * 1000
                , double(t1 - t0) / frame_count
                , wastage / frame_count
            );
            t0 = t1;
            frame_count = 0;
            wastage = 0;
        }

        // Waste the rest of the frame time...
        double now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        double waste_start = now_s;
        while (now_s < next_frame_start_s) {
            SDL_Delay(0);
            //std::this_thread::yield();
            now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        }
        next_frame_start_s += target_frame_time_s;
        wastage += 1000 * (now_s - waste_start);
    }
    ::printf("\n");

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
