
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
