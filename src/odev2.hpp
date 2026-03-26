#ifndef ODEV2_HPP
#define ODEV2_HPP

#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "odev1.hpp" // Image yapisi ve get_pixel gibi yapısal gereklilikler için

// ============================================================================
//  En Yakin Komsu (Nearest Neighbor) Interpolasyonu ve Boyutlandirma
// ============================================================================
inline float nn_interpolate(const Image& im, float x, float y, int c) {
    int nx = (int)std::round(x);
    int ny = (int)std::round(y);
    return get_pixel(im, nx, ny, c);
}

inline Image nn_resize(const Image& im, int w, int h) {
    Image ret;
    ret.w = w; ret.h = h; ret.c = im.c;
    ret.data = new float[w * h * im.c]();
    
    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;
    
    for (int k = 0; k < im.c; ++k) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                // Piksel merkezlerinden koordinat esleme
                float in_x = (x + 0.5f) * scale_x - 0.5f;
                float in_y = (y + 0.5f) * scale_y - 0.5f;
                float v = nn_interpolate(im, in_x, in_y, k);
                set_pixel(ret, x, y, k, v);
            }
        }
    }
    printf("  [OK] Nearest Neighbor Resize uygulandi. Yeni Boyut: %dx%d\n", w, h);
    return ret;
}

// ============================================================================
//  Cift Dogrusal (Bilinear) Interpolasyon ve Boyutlandirma
// ============================================================================
inline float bilinear_interpolate(const Image& im, float x, float y, int c) {
    int lx = (int)std::floor(x);
    int ux = lx + 1;
    int ly = (int)std::floor(y);
    int uy = ly + 1;
    
    float dx = x - (float)lx;
    float dy = y - (float)ly;
    
    float v1 = get_pixel(im, lx, ly, c); // sol-ust
    float v2 = get_pixel(im, ux, ly, c); // sag-ust
    float v3 = get_pixel(im, lx, uy, c); // sol-alt
    float v4 = get_pixel(im, ux, uy, c); // sag-alt
    
    // ust ve alt pikselleri yatayda enterpole et
    float t = v1 * (1.0f - dx) + v2 * dx;
    float b = v3 * (1.0f - dx) + v4 * dx;
    
    // dikeyde enterpole et
    return t * (1.0f - dy) + b * dy;
}

inline Image bilinear_resize(const Image& im, int w, int h) {
    Image ret;
    ret.w = w; ret.h = h; ret.c = im.c;
    ret.data = new float[w * h * im.c]();
    
    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;
    
    for (int k = 0; k < im.c; ++k) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                float in_x = (x + 0.5f) * scale_x - 0.5f;
                float in_y = (y + 0.5f) * scale_y - 0.5f;
                float v = bilinear_interpolate(im, in_x, in_y, k);
                set_pixel(ret, x, y, k, v);
            }
        }
    }
    printf("  [OK] Bilinear Resize uygulandi. Yeni Boyut: %dx%d\n", w, h);
    return ret;
}

// ============================================================================
//  Kutu Filtresi (Box Filter) ve Konvolusyon
// ============================================================================
inline void l1_normalize(Image& im) {
    float sum = 0.0f;
    int total = im.w * im.h * im.c;
    for (int i = 0; i < total; ++i) {
        sum += im.data[i];
    }
    if (sum != 0.0f) {
        for (int i = 0; i < total; ++i) {
            im.data[i] /= sum;
        }
    } else {
        // Toplam 0 ise, uniform (esit) agirliklandirmaya geri don
        for (int i = 0; i < total; ++i) {
            im.data[i] = 1.0f / total;
        }
    }
}

inline Image make_box_filter(int w) {
    Image ret;
    ret.w = w; ret.h = w; ret.c = 1;
    ret.data = new float[w * w]();
    for (int i = 0; i < w * w; ++i) {
        ret.data[i] = 1.0f;
    }
    l1_normalize(ret);
    return ret;
}

inline Image convolve_image(const Image& im, const Image& filter, int preserve) {
    Image ret;
    ret.w = im.w;
    ret.h = im.h;
    ret.c = preserve ? im.c : 1;
    ret.data = new float[ret.w * ret.h * ret.c]();
    
    int fc = filter.c;
    
    for (int c = 0; c < im.c; ++c) {
        int filter_c = (fc == im.c) ? c : 0;
        int ret_c = preserve ? c : 0;
        
        for (int y = 0; y < im.h; ++y) {
            for (int x = 0; x < im.w; ++x) {
                float sum = 0.0f;
                
                for (int fy = 0; fy < filter.h; ++fy) {
                    for (int fx = 0; fx < filter.w; ++fx) {
                        int im_x = x - filter.w / 2 + fx;
                        int im_y = y - filter.h / 2 + fy;
                        float im_v = get_pixel(im, im_x, im_y, c);
                        float f_v = get_pixel(filter, fx, fy, filter_c);
                        sum += im_v * f_v;
                    }
                }
                
                if (preserve) {
                    set_pixel(ret, x, y, ret_c, sum);
                } else {
                    float existing = get_pixel(ret, x, y, ret_c);
                    set_pixel(ret, x, y, ret_c, existing + sum);
                }
            }
        }
    }
    return ret;
}

// Temel Uzaysal Filtreler (Spatial Filters)
inline Image make_highpass_filter() {
    Image f; f.w = 3; f.h = 3; f.c = 1;
    f.data = new float[9]{0, -1, 0, -1, 4, -1, 0, -1, 0};
    return f;
}

inline Image make_sharpen_filter() {
    Image f; f.w = 3; f.h = 3; f.c = 1;
    f.data = new float[9]{0, -1, 0, -1, 5, -1, 0, -1, 0};
    return f;
}

inline Image make_emboss_filter() {
    Image f; f.w = 3; f.h = 3; f.c = 1;
    f.data = new float[9]{-2, -1, 0, -1, 1, 1, 0, 1, 2};
    return f;
}

// ============================================================================
//  Gauss Filtresi (Gaussian Filter)
// ============================================================================
inline Image make_gaussian_filter(float sigma) {
    int w = (int)std::ceil(sigma * 6.0f);
    if (w % 2 == 0) w++; 
    if (w < 1) w = 1;
    
    Image ret; ret.w = w; ret.h = w; ret.c = 1;
    ret.data = new float[w * w]();
    
    int center = w / 2;
    for (int y = 0; y < w; ++y) {
        for (int x = 0; x < w; ++x) {
            int dx = x - center;
            int dy = y - center;
            float g = expf(-(dx * dx + dy * dy) / (2.0f * sigma * sigma));
            ret.data[y * w + x] = g;
        }
    }
    l1_normalize(ret);
    return ret;
}

// ============================================================================
//  Hibrit Goruntuler (Hybrid Images)
// ============================================================================
inline Image add_image(const Image& a, const Image& b) {
    if (a.w != b.w || a.h != b.h || a.c != b.c) return a;
    Image ret;
    ret.w = a.w; ret.h = a.h; ret.c = a.c;
    int total = a.w * a.h * a.c;
    ret.data = new float[total];
    for (int i = 0; i < total; ++i) {
        ret.data[i] = a.data[i] + b.data[i];
    }
    return ret;
}

inline Image sub_image(const Image& a, const Image& b) {
    if (a.w != b.w || a.h != b.h || a.c != b.c) return a;
    Image ret;
    ret.w = a.w; ret.h = a.h; ret.c = a.c;
    int total = a.w * a.h * a.c;
    ret.data = new float[total];
    for (int i = 0; i < total; ++i) {
        ret.data[i] = a.data[i] - b.data[i];
    }
    return ret;
}

// ============================================================================
//  Sobel Operatoru
// ============================================================================
inline Image make_gx_filter() {
    Image f; f.w = 3; f.h = 3; f.c = 1;
    f.data = new float[9]{-1, 0, 1, -2, 0, 2, -1, 0, 1};
    return f;
}

inline Image make_gy_filter() {
    Image f; f.w = 3; f.h = 3; f.c = 1;
    f.data = new float[9]{-1, -2, -1, 0, 0, 0, 1, 2, 1};
    return f;
}

inline void feature_normalize(Image& im) {
    if (im.data == nullptr) return;
    int total = im.w * im.h * im.c;
    float min_v = im.data[0];
    float max_v = im.data[0];
    
    for (int i = 1; i < total; ++i) {
        if (im.data[i] < min_v) min_v = im.data[i];
        if (im.data[i] > max_v) max_v = im.data[i];
    }
    
    float range = max_v - min_v;
    if (range < 1e-6f) {
        for (int i = 0; i < total; ++i) im.data[i] = 0.0f;
    } else {
        for (int i = 0; i < total; ++i) {
            im.data[i] = (im.data[i] - min_v) / range;
        }
    }
}

// 2 goruntuluk bir diziye isaretci dondurur: [0] = buyukluk (magnitude), [1] = yon (direction)
inline Image* sobel_image(const Image& im) {
    Image* ret = new Image[2]; // Manuel olarak silinmelidir
    
    Image gx_filter = make_gx_filter();
    Image gy_filter = make_gy_filter();
    
    Image gx = convolve_image(im, gx_filter, 0); // Korumali = 0 (1 kanal)
    Image gy = convolve_image(im, gy_filter, 0); 
    
    Image mag; mag.w = im.w; mag.h = im.h; mag.c = 1;
    mag.data = new float[mag.w * mag.h]();
    
    Image dir; dir.w = im.w; dir.h = im.h; dir.c = 1;
    dir.data = new float[dir.w * dir.h]();
    
    int wh = im.w * im.h;
    for (int i = 0; i < wh; ++i) {
        float vx = gx.data[i];
        float vy = gy.data[i];
        mag.data[i] = std::sqrt(vx * vx + vy * vy);
        dir.data[i] = std::atan2(vy, vx);
    }
    
    ret[0] = mag;
    ret[1] = dir;
    return ret;
}

inline Image colorize_sobel(const Image& im) {
    Image* sobel = sobel_image(im);
    Image mag = sobel[0];
    Image dir = sobel[1];
    
    feature_normalize(mag);
    
    Image hsv; hsv.w = im.w; hsv.h = im.h; hsv.c = 3;
    hsv.data = new float[hsv.w * hsv.h * 3]();
    
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            float d = dir.data[y * im.w + x];
            // -PI ile PI arasindaki yonu -> 0 ile 360 arasina esle
            float h = (d / (2.0f * (float)M_PI) + 0.5f) * 360.0f; 
            float s = 1.0f; 
            float v = mag.data[y * im.w + x]; 
            
            set_pixel(hsv, x, y, 0, h);
            set_pixel(hsv, x, y, 1, s);
            set_pixel(hsv, x, y, 2, v);
        }
    }
    
    // Dizi elemanlarinin sizinti yapmamasi icin temizle
    delete[] sobel; 
    
    // Dogrudan RGB'ye donustur
    hsv_to_rgb(hsv);
    clamp_image(hsv);
    
    printf("  [OK] Sobel Edge Detection uygulandi.\n");
    return hsv;
}

#endif // ODEV2_HPP
