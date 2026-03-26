#ifndef ODEV1_HPP
#define ODEV1_HPP

#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstring>

// ============================================================================
//  Image Yapisi - CHW Planar Duzeni
// ============================================================================
struct Image {
    float* data;  // CHW planar piksel verisi
    int w;        // Genislik
    int h;        // Yukseklik
    int c;        // Kanal sayisi

    Image() : data(nullptr), w(0), h(0), c(0) {}

    ~Image() {
        if (data) {
            delete[] data;
            data = nullptr;
        }
    }

    // Kopyalama Yapicisi (Copy Constructor) - derin kopya
    Image(const Image& other) : w(other.w), h(other.h), c(other.c) {
        int total = w * h * c;
        if (total > 0 && other.data) {
            data = new float[total];
            std::memcpy(data, other.data, total * sizeof(float));
        } else {
            data = nullptr;
        }
    }

    // Kopyalama Atama Operatoru (Copy Assignment) - derin kopya
    Image& operator=(const Image& other) {
        if (this != &other) {
            delete[] data;
            w = other.w;
            h = other.h;
            c = other.c;
            int total = w * h * c;
            if (total > 0 && other.data) {
                data = new float[total];
                std::memcpy(data, other.data, total * sizeof(float));
            } else {
                data = nullptr;
            }
        }
        return *this;
    }
};

// ============================================================================
//  #1: get_pixel ve set_pixel (Piksel alma ve atama)
// ============================================================================
inline float get_pixel(const Image& img, int x, int y, int c) {
    if (x < 0) x = 0; else if (x >= img.w) x = img.w - 1;
    if (y < 0) y = 0; else if (y >= img.h) y = img.h - 1;
    if (c < 0) c = 0; else if (c >= img.c) c = img.c - 1;
    return img.data[c * (img.w * img.h) + y * img.w + x];
}

inline void set_pixel(Image& img, int x, int y, int c, float v) {
    if (x < 0 || x >= img.w || y < 0 || y >= img.h || c < 0 || c >= img.c) return;
    img.data[c * (img.w * img.h) + y * img.w + x] = v;
}

// ============================================================================
//  #2: copy_image (Goruntu kopyalama)
// ============================================================================
inline Image copy_image(const Image& img) {
    return Image(img); // Yukarida tanimlanan kopyalama yapicisini kullanir
}

// ============================================================================
//  #3: rgb_to_grayscale (RGB'den Gri Tona Donusum)
// ============================================================================
inline void rgb_to_grayscale(Image& img) {
    if (img.data == nullptr || img.c < 3) return;
    int wh = img.w * img.h;
    
    for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
            float r = get_pixel(img, x, y, 0);
            float g = get_pixel(img, x, y, 1);
            float b = get_pixel(img, x, y, 2);
            
            float l = 0.2126f * r + 0.7152f * g + 0.0722f * b;
            
            set_pixel(img, x, y, 0, l);
            set_pixel(img, x, y, 1, l);
            set_pixel(img, x, y, 2, l);
        }
    }
    printf("\n  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║  rgb_to_grayscale GOREVI TAMAMLANDI                    ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║  Islenen piksel sayisi: %-30d ║\n", wh);
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
}

// ============================================================================
//  #4: shift_image (Degerleri kaydirma/parlaklik artirma)
// ============================================================================
inline void shift_image(Image& img, float shift) {
    if (img.data == nullptr) return;
    int total = img.w * img.h * img.c;
    for (int i = 0; i < total; ++i) {
        img.data[i] += shift;
    }
    printf("\n  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║  shift_image GOREVI TAMAMLANDI                         ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║  Eklenen deger: %-+10.4f                               ║\n", shift);
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
}

// ============================================================================
//  #5: clamp_image (Degerleri 0 ile 1 arasinda tutma)
// ============================================================================
inline void clamp_image(Image& img) {
    if (img.data == nullptr) return;
    int total = img.w * img.h * img.c;
    for (int i = 0; i < total; ++i) {
        if (img.data[i] < 0.0f) img.data[i] = 0.0f;
        else if (img.data[i] > 1.0f) img.data[i] = 1.0f;
    }
    printf("  [OK] clamp_image calisti => Degerler [0,1] arasinda tutuldu.\n");
}

// ============================================================================
//  #6: rgb_to_hsv (RGB'den HSV'ye Donusum)
// ============================================================================
inline void rgb_to_hsv(Image& img) {
    if (img.data == nullptr || img.c < 3) return;
    int wh = img.w * img.h;

    for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
            float r = get_pixel(img, x, y, 0);
            float g = get_pixel(img, x, y, 1);
            float b = get_pixel(img, x, y, 2);

            float cmax = std::max({r, g, b});
            float cmin = std::min({r, g, b});
            float delta = cmax - cmin;

            float h = 0.0f, s = 0.0f, v = cmax;

            if (delta > 1e-6f) {
                s = delta / cmax;

                if (cmax == r)
                    h = 60.0f * fmodf((g - b) / delta, 6.0f);
                else if (cmax == g)
                    h = 60.0f * ((b - r) / delta + 2.0f);
                else
                    h = 60.0f * ((r - g) / delta + 4.0f);

                if (h < 0.0f) h += 360.0f;
            }

            set_pixel(img, x, y, 0, h);
            set_pixel(img, x, y, 1, s);
            set_pixel(img, x, y, 2, v);
        }
    }
    
    printf("\n  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║  rgb_to_hsv GOREVI TAMAMLANDI                          ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║  Islenen piksel: %-34d ║\n", wh);
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
}

// ============================================================================
//  #7: hsv_to_rgb (HSV'den RGB'ye Donusum)
// ============================================================================
inline void hsv_to_rgb(Image& img) {
    if (img.data == nullptr || img.c < 3) return;
    int wh = img.w * img.h;

    for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
            float h = get_pixel(img, x, y, 0);
            float s = get_pixel(img, x, y, 1);
            float v = get_pixel(img, x, y, 2);

            float c = v * s;
            float px = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
            float m = v - c;

            float r = 0, g = 0, b = 0;

            if      (h < 60.0f)  { r = c; g = px; b = 0; }
            else if (h < 120.0f) { r = px; g = c; b = 0; }
            else if (h < 180.0f) { r = 0; g = c; b = px; }
            else if (h < 240.0f) { r = 0; g = px; b = c; }
            else if (h < 300.0f) { r = px; g = 0; b = c; }
            else                 { r = c; g = 0; b = px; }

            set_pixel(img, x, y, 0, r + m);
            set_pixel(img, x, y, 1, g + m);
            set_pixel(img, x, y, 2, b + m);
        }
    }

    printf("\n  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║  hsv_to_rgb GOREVI TAMAMLANDI                          ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║  Islenen piksel: %-34d ║\n", wh);
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
}

#endif // ODEV1_HPP
