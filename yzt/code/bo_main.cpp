#include <sdl2/SDL.h>
#include <cmath>
#include <cstdio>

using byte = unsigned char;

struct Color {
    byte b, g, r, a;

    Color (byte r_, byte g_, byte b_, byte a_ = 255) : r (r_), g (g_), b (b_), a (a_) {}
};

struct Canvas {
    void * pixels_raw;
    int pitch_bytes;
    int width, height;

    Color * pixel(int x, int y) {return (Color *)((byte *)pixels_raw + y * (size_t)pitch_bytes + x * sizeof(Color));}
};

void Render_Pixel (Canvas * canvas, int x, int y, Color c) {
    if (canvas && x >= 0 && y >= 0 && x < canvas->width && y < canvas->height)
        *canvas->pixel(x, y) = c;
}

void Render_LineHoriz_Unchecked (Canvas * canvas, int x0, int x1, int y, Color c) {
    Color * p = canvas->pixel(x0, y);
    for (int i = x1 - x0; i >= 0; --i, ++p)
        *p = c;
}

void Render_LineHoriz (Canvas * canvas, int x0, int x1, int y, Color c) {
    if (canvas && y >= 0 && y < canvas->height) {
        if (x1 < x0) {auto t = x0; x0 = x1; x1 = t;}
        if (x0 < 0) x0 = 0;
        if (x1 > canvas->width - 1) x1 = canvas->width - 1;
        if (x0 <= x1)
            Render_LineHoriz_Unchecked(canvas, x0, x1, y, c);
    }
}

void Render_AAB (Canvas * canvas, int x0, int y0, int w, int h, Color c) {
    if (canvas && w > 0 && h > 0 && x0 < canvas->width && y0 < canvas->height && x0 + w >= 0 && y0 + h >= 0) {
        if (x0 < 0) {w += x0; x0 = 0;}
        if (y0 < 0) {h += y0; y0 = 0;}
        if (x0 + w >= canvas->width) {w -= x0 + w - canvas->width;}
        if (y0 + h >= canvas->height) {h -= y0 + h - canvas->height;}

        Color * p = canvas->pixel(x0, y0);
        for (int i = 0; i < h; ++i, p = (Color *)((byte *)p + canvas->pitch_bytes)) {
            Color * q = p;
            for (int j = 0; j < w; ++q, ++j) {
                *q = c;
            }
        }
    }
}

void Render_Clear (Canvas * canvas, Color c) {
    Render_AAB(canvas, 0, 0, canvas->width, canvas->height, c);
}

void Render_Circle (Canvas * canvas, int x, int y, int r, Color c) {
    if (canvas && r >= 0) {
        for (int ey = r - 1; ey > 0; --ey) {
            int ex = int(0.5f + sqrtf(float(r * r - ey * ey)));
            Render_LineHoriz(canvas, x - ex, x + ex, y + ey, c);
            Render_LineHoriz(canvas, x - ex, x + ex, y - ey, c);
        }
        Render_LineHoriz(canvas, x - r, x + r, y, c);
        Render_Pixel(canvas, x, y + r, c);
        Render_Pixel(canvas, x, y - r, c);
    }
}

int main (int argc, char * argv []) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_CreateWindowAndRenderer(1280, 720, 0 /*| SDL_WINDOW_FULLSCREEN*/, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 142, 0, 0, 255);

    int window_width = 0, window_height = 0;
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_Texture * tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    SDL_assert(tex);

    float paddle_x = 0;
    float paddle_speed = 500.0f;
    float movement = 0.0f;  // in [-1..1]
    bool input_left = false;
    bool input_right = false;
    bool input_exit = false;
    int config_target_fps = 60;

    double target_frame_time_s = 1.0 / config_target_fps;
    double const inv_pfc_freq = 1.0 / SDL_GetPerformanceFrequency();
    double next_frame_start_s = inv_pfc_freq * SDL_GetPerformanceCounter() + target_frame_time_s;

    SDL_Event ev = {};
    unsigned t0 = SDL_GetTicks();
    unsigned frame_count = 0;
    while (!input_exit) {
        // Process pending events...
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                case SDLK_a:
                case SDLK_LEFT:
                    input_left = true;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    input_right = true;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE:
                    input_exit = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    input_left = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    input_right = false;
                    break;
                }
                break;
            case SDL_QUIT:
                input_exit = true;
                break;
            default:
                break;
            }
        }

        // Process the input...
        if (input_left && !input_right)
            movement = -1.0f;
        else if (input_right && !input_left)
            movement = 1.0f;
        else
            movement = 0.0f;

        // Do the update...
        paddle_x += movement * paddle_speed * float(target_frame_time_s);

        // Do the render...
        Canvas canvas = {};
        SDL_LockTexture(tex, nullptr, &canvas.pixels_raw, &canvas.pitch_bytes);
        canvas.width = window_width;
        canvas.height = window_height;

        Render_Clear(&canvas, {0, 0, 0});

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

        Render_Circle(&canvas, 800, 200, 100, {0, 255, 0});
        Render_Circle(&canvas, 950, 200,  50, {0, 0, 255});

        Render_AAB (&canvas, int(paddle_x + 0.5f), 300, 160, 40, {255, 0, 0});

        SDL_UnlockTexture(tex);

        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // FPS counter ...
        frame_count += 1;
        unsigned t1 = SDL_GetTicks();
        if (t1 - t0 >= 1 * 1000) {
            ::printf("Average FPS = %5.1f, average frame time = %5.1f ms\r"
                , double(frame_count) / (t1 - t0) * 1000
                , double(t1 - t0) / frame_count
            );
            t0 = t1;
            frame_count = 0;
        }

        // Waste the rest of the frame time...
        double now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        while (now_s < next_frame_start_s) {
            SDL_Delay(0);
            now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        }
        next_frame_start_s += target_frame_time_s;
    }
    ::printf("\n");

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
