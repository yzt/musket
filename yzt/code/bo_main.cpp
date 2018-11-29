#include <sdl2/SDL.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#define DRAW_BALL_HISTORY

#if defined(NDEBUG)
    #define ASSERT(cond, ...)   ((void)(cond))
#else
    #define ASSERT(cond, ...)   assert(cond)
#endif
using byte = unsigned char;

#include "bo_math.hpp"
#include "bo_render.hpp"

struct Config {
    int target_fps = 120;
    int window_width = 600;
    int window_height = 0;
    float window_aspect_ratio = 3.0f / 4.0f;

    float paddle_speed = 1000.0f;
    float paddle_vert_pos = 0.90f;
    Vec2f paddle_half_dims = {80, 10};
    float ball_radius = 42.0f;  //10.0f;
    float ball_speed = 1200.0f;
};

struct Input {
    float movement = 0.0f; // in [-1..1]
    bool exit = false;
    bool action = false;

    bool left_pressed = false;
    bool right_pressed = false;
};

struct State {
    Point2f paddle_pos = {300, 300};
    Point2f ball_pos = {};
    Vec2f ball_dir = {};
    bool ball_in_movement = false;
};


struct CollisionResult {
    bool exists;
    Real param;
    Point2f point;
    Vec2f normal;
};

CollisionResult Collide_BallPaddle (
    Point2f const & ball_pos, Real ball_radius, Vec2f const & ball_movement,   // ball_dir * ball_speed * time_step
    Point2f const & paddle_pos, Vec2f const & paddle_half_dims, Vec2f const & paddle_movement
) {
    CollisionResult ret = {};
    //ret.param = 2.0f;   // +Inf
    auto movement = ball_movement - paddle_movement;
    auto ball_expected = ball_pos + movement;

    Point2f corners [4] = {
        {paddle_pos.x - paddle_half_dims.x, paddle_pos.y - paddle_half_dims.y},
        {paddle_pos.x - paddle_half_dims.x, paddle_pos.y + paddle_half_dims.y},
        {paddle_pos.x + paddle_half_dims.x, paddle_pos.y + paddle_half_dims.y},
        {paddle_pos.x + paddle_half_dims.x, paddle_pos.y - paddle_half_dims.y},
    };
    Vec2f normals [4] = {
        {-1.0f, 0},
        {0, +1.0f},
        {+1.0f, 0},
        {0, -1.0f},
    };
    for (int i = 0; i < 4; ++i) {
        auto displacement = ball_radius * normals[i];
        auto m0 = corners[i] + displacement;
        auto m1 = corners[(i + 1) % 4] + displacement;
        auto c = Intersect_LineLine(ball_pos, ball_expected, m0, m1);
        if (c.exists && c.l_param > 0 && c.l_param <= 1.0f && c.m_param >= 0 && c.m_param <= 1.0f) {
            if (!ret.exists || c.l_param < ret.param) {
                ret.exists = true;
                ret.param = c.l_param;
                //ret.point = Lerp(m0, m1, c.m_param);
                ret.point = Lerp(ball_pos, ball_pos + ball_movement, c.l_param);
                ret.normal = normals[i];
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        auto c = Intersect_LineCircle(ball_pos, ball_expected, corners[i], ball_radius);
        if (c.count >= 2) {
            if (c.param1 <= 0 || (c.param2 > 0 && c.param2 < c.param1))
                c.param1 = c.param2;
            c.count = 1;
        }
        if (c.count >= 1 && c.param1 > 0 && c.param1 <= 1.0f) {
            if (!ret.exists || c.param1 < ret.param) {
                ret.exists = true;
                ret.param = c.param1;
                ret.point = Lerp(ball_pos, ball_pos + ball_movement, c.param1);
                ret.normal = Normalize(ret.point - corners[i]);
            }
        }
    }

    return ret;
}

int main (int argc, char * argv []) {
    Config config;
    Input input;
    State state;

    config.window_height = Round(config.window_width / config.window_aspect_ratio);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_CreateWindowAndRenderer(config.window_width, config.window_height, 0 /*| SDL_WINDOW_FULLSCREEN*/, &window, &renderer);
    SDL_GetWindowSize(window, &config.window_width, &config.window_height);
    SDL_SetWindowTitle(window, "BrykOut");

    SDL_Texture * tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, config.window_width, config.window_height);
    SDL_assert(tex);

    state.paddle_pos = {
        0.5f * config.window_width,
        config.paddle_vert_pos * config.window_height
    };

    double target_frame_time_s = 1.0 / config.target_fps;
    double const inv_pfc_freq = 1.0 / SDL_GetPerformanceFrequency();
    double next_frame_start_s = inv_pfc_freq * SDL_GetPerformanceCounter() + target_frame_time_s;
    double wastage = 0.0;

    #if defined(DRAW_BALL_HISTORY)
    std::vector<Point2f> ball_history;
    #endif

    SDL_Event ev = {};
    unsigned t0 = SDL_GetTicks();
    unsigned frame_count = 0;
    for (;;) {
        // Process pending events...
        input.action = false;
        input.exit = false;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                case SDLK_a: case SDLK_LEFT: input.left_pressed = true; break;
                case SDLK_d: case SDLK_RIGHT: input.right_pressed = true; break;
                case SDLK_SPACE: input.action = true;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                case SDLK_a: case SDLK_LEFT: input.left_pressed = false; break;
                case SDLK_d: case SDLK_RIGHT: input.right_pressed = false; break;
                case SDLK_ESCAPE: input.exit = true; break;
                }
                break;
            case SDL_QUIT:
                input.exit = true;
                break;
            }
        }

        // Process the input...
        if (input.exit)
            break;

        if (input.left_pressed && !input.right_pressed)
            input.movement = -1.0f;
        else if (input.right_pressed && !input.left_pressed)
            input.movement = 1.0f;
        else
            input.movement = 0.0f;

        if (input.action && !state.ball_in_movement) {
            state.ball_in_movement = true;
            state.ball_dir = Normalize({(input.movement >= 0 ? 1.0f : -1.0f), -1.0f});
        #if defined(DRAW_BALL_HISTORY)
            ball_history.clear();
            ball_history.push_back(state.ball_pos);
        #endif
        }

        // Do the update...
        State next = state;
        next.paddle_pos.x += input.movement * config.paddle_speed * float(target_frame_time_s);
        if (next.paddle_pos.x < config.paddle_half_dims.x)
            next.paddle_pos.x = config.paddle_half_dims.x;
        if (next.paddle_pos.x > config.window_width - config.paddle_half_dims.x)
            next.paddle_pos.x = config.window_width - config.paddle_half_dims.x;
        
        if (!state.ball_in_movement) {
            next.ball_pos = {
                next.paddle_pos.x,
                next.paddle_pos.y - config.paddle_half_dims.y - config.ball_radius
            };
        } else {
            Real const R = config.ball_radius;
            Point2f const corners [4] = {
                {0 + R, 0 + R},
                {0 + R, config.window_height - R},
                {config.window_width - R, config.window_height - R},
                {config.window_width - R, 0 + R},
            };
            Vec2f const normals [4] = {
                {+1.0f, 0.0f},
                { 0.0f,-1.0f},
                {-1.0f, 0.0f},
                { 0.0f,+1.0f},
            };

            //next.ball_pos = state.ball_pos + state.ball_dir * (config.ball_speed * float(target_frame_time_s));

            Real rem = 1.0f;
            auto bp = next.ball_pos;
            auto bd = next.ball_dir;

            //if (next.ball_pos.y + config.ball_radius <= next.paddle_pos.y - config.paddle_half_dims.y) {
                auto paddle_collision = Collide_BallPaddle(
                    state.ball_pos, config.ball_radius, state.ball_dir * (config.ball_speed * float(target_frame_time_s) * rem),
                    state.paddle_pos, config.paddle_half_dims, next.paddle_pos - state.paddle_pos
                );
                if (paddle_collision.exists) {
                    bp = paddle_collision.point;
                    bd = Normalize(Reflect(bd, paddle_collision.normal));
                    rem -= paddle_collision.param * rem;
                    #if defined(DRAW_BALL_HISTORY)
                        ball_history.push_back(paddle_collision.point);
                    #endif
                }
            //}

            while (rem > 0.001f) {
                auto ep = bp + bd * (config.ball_speed * float(target_frame_time_s) * rem);
                bool collides_with_walls = false;
                for (int i = 0; i < 4; ++i) {
                    auto r = Intersect_LineLine(bp, ep, corners[i], corners[(i + 1) % 4]);
                    if (r.exists && r.l_param > 0 && r.l_param <= 1.0f && r.m_param >= 0 && r.m_param <= 1.0f) {
                        auto cp = Lerp(corners[i], corners[(i + 1) % 4], r.m_param);
                        auto cn = normals[i];
                        auto cd = Normalize(Reflect(bd, cn));
                        auto ct = r.l_param;

                        bp = cp;
                        bd = cd;
                    #if defined(DRAW_BALL_HISTORY)
                        ball_history.push_back(cp);
                    #endif

                        rem -= ct * rem;
                        //rem = (1 - ct) * rem;
                        collides_with_walls = true;

                        if (1 == i) {
                            // Lost the ball!
                            next.ball_in_movement = false;
                        }
                        break;
                    }
                }
                if (!collides_with_walls) {
                    bp = ep;
                    rem = 0.0f;
                }
            }
            next.ball_pos = bp;
            next.ball_dir = bd;
        #if defined(DRAW_BALL_HISTORY)
            ball_history.push_back(bp);
        #endif
        }
        
        state = next;

        // Do the render...
        Canvas canvas = {};
        SDL_LockTexture(tex, nullptr, &canvas.pixels_raw, &canvas.pitch_bytes);
        canvas.width = config.window_width;
        canvas.height = config.window_height;

        Render_Clear(&canvas, {0, 0, 0});

    #if defined(DRAW_BALL_HISTORY)
        for (unsigned i = 1; i < ball_history.size(); ++i)
            Render_Line(
                &canvas, 
                Round(ball_history[i - 1].x), Round(ball_history[i - 1].y),
                Round(ball_history[i - 0].x), Round(ball_history[i - 0].y),
                {0, 255, 255}
            );
        for (auto const & p : ball_history)
            Render_Circle(&canvas, Round(p.x), Round(p.y), 2, {0, 255, 255});
    #endif

        Render_AAB(
            &canvas,
            Round(state.paddle_pos.x - config.paddle_half_dims.x), Round(state.paddle_pos.y - config.paddle_half_dims.y),
            Round(2 * config.paddle_half_dims.x), Round(2 * config.paddle_half_dims.y),
            {255, 0, 0}
        );

        Render_Circle(
            &canvas,
            Round(state.ball_pos.x), Round(state.ball_pos.y),
            Round(config.ball_radius),
            {0, 255, 0}
        );

        SDL_UnlockTexture(tex);

        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // FPS counter ...
        frame_count += 1;
        unsigned param1 = SDL_GetTicks();
        if (param1 - t0 >= 1 * 1000) {
            char buffer [200];
            ::snprintf(buffer, sizeof(buffer)
                , "BrykOut    [FPS = %7.2f, frame time = %7.2fms, wastage = %7.2fms (%4.1f%%)]"
                , double(frame_count) / (param1 - t0) * 1000
                , double(param1 - t0) / frame_count
                , 1000 * wastage / frame_count
                , (1000 * wastage) / double(param1 - t0) * 100
            );
            SDL_SetWindowTitle(window, buffer);

            t0 = param1;
            frame_count = 0;
            wastage = 0;
        }

        // Waste the rest of the frame time...
        double now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        double waste_start = now_s;
        while (now_s < next_frame_start_s) {
            SDL_Delay(0);
            now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        }
        next_frame_start_s += target_frame_time_s;
        wastage += now_s - waste_start;
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
