
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

// ============================================================================
//  stb_image implementasyonu (sadece BU dosyada)
// ============================================================================
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image_write.h"

// ============================================================================
//  Odev header dosyalari (Image struct odev1.hpp icinde)
// ============================================================================
#include "odev1.hpp"
#include "odev2.hpp"

// ============================================================================
//  Goruntu Okuma (stb_image -> CHW planar, normalize)
// ============================================================================

Image load_image(const char* path) {
    Image img;
    int channels_in_file;

    // stb_image HWC interleaved olarak okur: RGBRGBRGB...
    unsigned char* raw = stbi_load(path, &img.w, &img.h, &channels_in_file, 3);
    img.c = 3; // Her zaman 3 kanal olarak oku

    if (!raw) {
        printf("  [HATA] Goruntu yuklenemedi: %s\n", path);
        printf("  [HATA] stb_image hatasi: %s\n", stbi_failure_reason());
        return img;
    }

    int wh = img.w * img.h;
    img.data = new float[wh * img.c];

    // HWC (interleaved) -> CHW (planar) donusumu + [0,255] -> [0,1] normalize
    for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
            int hw_idx = y * img.w + x;
            for (int ch = 0; ch < img.c; ++ch) {
                img.data[ch * wh + hw_idx] =
                    raw[(hw_idx) * img.c + ch] / 255.0f;
            }
        }
    }

    stbi_image_free(raw);

    printf("  [OK] Goruntu yuklendi: %s\n", path);
    printf("       Boyut: %dx%d | Kanal: %d | Bellek: %.2f KB\n",
           img.w, img.h, img.c,
           (wh * img.c * sizeof(float)) / 1024.0f);
    printf("       Bellek duzeni: CHW (Planar)\n");

    return img;
}

// ============================================================================
//  Goruntu Kaydetme (CHW planar -> stb_image_write)
// ============================================================================

bool save_image(const Image& img, const char* path) {
    if (!img.data) {
        printf("  [HATA] Kaydedilecek goruntu verisi bos.\n");
        return false;
    }

    int wh = img.w * img.h;

    // CHW (planar) -> HWC (interleaved) donusumu + [0,1] -> [0,255]
    unsigned char* out = new unsigned char[wh * img.c];

    for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
            int hw_idx = y * img.w + x;
            for (int ch = 0; ch < img.c; ++ch) {
                float val = img.data[ch * wh + hw_idx];
                // Sinirlama (Clamp) + normalizasyonu kaldir
                val = std::max(0.0f, std::min(1.0f, val));
                out[hw_idx * img.c + ch] = (unsigned char)(val * 255.0f + 0.5f);
            }
        }
    }

    // Uzantiya gore kaydet
    bool ok = false;
    std::string spath(path);

    if (spath.size() >= 4 && spath.substr(spath.size() - 4) == ".png") {
        ok = stbi_write_png(path, img.w, img.h, img.c, out, img.w * img.c) != 0;
    } else if (spath.size() >= 4 && spath.substr(spath.size() - 4) == ".bmp") {
        ok = stbi_write_bmp(path, img.w, img.h, img.c, out) != 0;
    } else {
        // Varsayilan: JPG (kalite 95)
        ok = stbi_write_jpg(path, img.w, img.h, img.c, out, 95) != 0;
    }

    delete[] out;

    if (ok) {
        printf("  [OK] Goruntu kaydedildi: %s\n", path);
    } else {
        printf("  [HATA] Goruntu kaydedilemedi: %s\n", path);
    }
    return ok;
}

// ============================================================================
//  Zaman Damgali Dosya Adi Uretme
// ============================================================================

std::string generate_timestamped_filename(const char* prefix, const char* ext) {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[256];
    // Dosya yolunda "/" yerine Windows uyumlu "\\" kullanımı (isteğe bağlı ama güvenli)
    snprintf(buf, sizeof(buf), "output/%s_%04d%02d%02d_%02d%02d%02d.%s",
             prefix,
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             ext);
    printf("  [DEBUG] Olusturulan dosya yolu: %s\n", buf);
    return std::string(buf);
}



// ============================================================================
//  Banner & Menu Gosterimi
// ============================================================================

void print_banner() {
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║     SAYISAL GORUNTU ISLEME - TALHA KESKIN     ║\n");
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_instructions() {
    printf("  [KULLANIM REHBERI]\n");
    printf("  1. YUKLEME: Program otomatik olarak 'input/giris.jpg' dosyasini okur.\n");
    printf("  2. KATMANLI ISLEM: Menuden peş peşe sayılar (örn: 1 sonra 2) sectiginizde,\n");
    printf("     islemler UST USTE ayni fotograf uzerine etki eder.\n");
    printf("  3. ORİJİNALE DÖN: Uyguladıgınız islemleri temizlemek ve resmi bastaki \n");
    printf("     haline geri dondurmek isterseniz her zaman 'r' tusuna basabilirsiniz.\n");
    printf("  4. PIKSEL TAKIBI: İslemlerden once ve sonra merkez pikselin nasil\n");
    printf("     degistigini gormek icin 'p' tusuna basip takibi acabilirsiniz.\n");
    printf("  5. KAYDETME: Olusan sonucu begendiginizde 's' tusuna basarsaniz fotograf,\n");
    printf("     'output/' klasorune zaman damgasiyla (sonuc_yilay_saat.jpg) kaydedilir.\n");
    printf("  6. CIKIS: İşiniz bittiginde 'q' tusuna basip kaydederek kapatabilirsiniz.\n\n");
}

void clear_screen() {
    // ANSI kacak kodlari: \033[H matrisi basa alir, \033[J ekrani temizler
    printf("\033[H\033[J");
}

void print_menu() {
    printf("\n");
    printf("  ┌──────────────────────────────────────────────────────────┐\n");
    printf("  │                    ISLEM MENUSU                         │\n");
    printf("  ├──────────────────────────────────────────────────────────┤\n");
    printf("  │  1. Grayscale   - Luminance formulu ile griye cevir     │\n");
    printf("  │  2. Shift       - Parlaklik artir/azalt (float deger)   │\n");
    printf("  │  3. HSV         - RGB -> HSV donusumu                   │\n");
    printf("  │  4. HSV -> RGB  - HSV -> RGB geri donusum               │\n");
    printf("  ├──────────────────────────────────────────────────────────┤\n");
    printf("  │  5. NN Resize   - Yakin Komsu (Buyutme / Kucultme)      │\n");
    printf("  │  6. BL Resize   - Bilinear (Buyutme / Kucultme)         │\n");
    printf("  │  7. Box         - Box Filter / Blur (7x7)               │\n");
    printf("  │  8. Sharpen     - Keskinlestirme (Convolution)          │\n");
    printf("  │  9. Gaussian    - Gaussian Blur (Sigma=2)               │\n");
    printf("  │  0. Sobel       - Sobel Edge Detection ve Boyama        │\n");
    printf("  ├──────────────────────────────────────────────────────────┤\n");
    printf("  │  p. Piksel Degisimini Goster/Gizle (Ac/Kapa)            │\n");
    printf("  │  s. Kaydet ve Devam Et                                  │\n");
    printf("  │  r. Resmi Orijinale Döndür (input/giris.jpg yükle)      │\n");
    printf("  │  q. Kaydet ve Cik                                       │\n");
    printf("  └──────────────────────────────────────────────────────────┘\n");
    printf("\n  Lütfen test etmek istediğiniz işlemin tuşuna basıp Enter'a basınız: ");
}

// ============================================================================
//  Ana Program
// ============================================================================

int main() {
    clear_screen();
    print_banner();
    print_instructions();

    printf("  >>> Programa baslamak icin lutfen [ENTER] tusuna basin... ");
    int start_ch; while ((start_ch = getchar()) != '\n' && start_ch != EOF);
    printf("\n");

    // ----- Giris goruntusu otomatik yukleme -----
    const char* input_path = "input/giris.jpg";
    printf("  [BILGI] Giris goruntusu araniyor: %s\n", input_path);

    Image img = load_image(input_path);
    if (!img.data) {
        printf("\n  [HATA] '%s' bulunamadi veya okunamadi.\n", input_path);
        printf("  [BILGI] Lutfen 'input/giris.jpg' dosyasini olusturun.\n");
        return 1;
    }

    // Islenmis goruntunun kopyasi (kaydetme icin)
    int islem_sayaci = 0;
    std::vector<std::string> islem_gecmisi;
    bool goster_piksel = false;

    // ----- Interaktif menu dongusu -----
    char secim[16];
    bool devam = true;

    while (devam) {
        clear_screen();
        print_banner();

        if (!islem_gecmisi.empty()) {
            printf("\n  [GECMIS] Uygulanan Islemler: ");
            for (size_t i = 0; i < islem_gecmisi.size(); ++i) {
                printf("%s", islem_gecmisi[i].c_str());
                if (i != islem_gecmisi.size() - 1) printf(" -> ");
            }
            printf("\n");
        }

        print_menu();

        if (!fgets(secim, sizeof(secim), stdin)) break;

        // Yeni satir karakterini kaldir
        secim[strcspn(secim, "\n")] = '\0';

        int prev_islem_sayaci = islem_sayaci;
        float before_p[3] = {0};
        int get_px_w = img.w, get_px_h = img.h, get_px_c = img.c;
        if (goster_piksel && img.data && img.w > 0 && img.h > 0) {
            int cx = img.w / 2;
            int cy = img.h / 2;
            int idx = cy * img.w + cx;
            int wh = img.w * img.h;
            for (int c = 0; c < img.c && c < 3; ++c) {
                before_p[c] = img.data[c * wh + idx];
            }
        }
        std::string uygulanan_islem = "";

        if (strcmp(secim, "1") == 0) {
            // ---- GRI TON (GRAYSCALE) ----
            printf("\n  [ACIKLAMA] Görüntüdeki her pikselin RGB değerleri ITU-R formülüyle \n");
            printf("             tek bir gri tona indirgeniyor...\n");
            rgb_to_grayscale(img);
            uygulanan_islem = "Grayscale";
            islem_sayaci++;

        } else if (strcmp(secim, "2") == 0) {
            // ---- PARLAKLIK KAYDIRMA (SHIFT) ----
            float shift;
            printf("  Shift degeri girin (orn: 0.2 veya -0.1): ");
            if (scanf("%f", &shift) != 1) {
                printf("  [HATA] Gecersiz deger.\n");
                // stdin temizle
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue;
            }
            // stdin'deki kalan newline'i temizle
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            printf("\n  [ACIKLAMA] Piksel değerlerine %f ekleniyor ve Görev 5 (clamp) uygulanıyor...\n", shift);
            shift_image(img, shift);
            clamp_image(img); // Görev 5
            char op_buf[64];
            snprintf(op_buf, sizeof(op_buf), "Shift(%.2f)", shift);
            uygulanan_islem = op_buf;
            islem_sayaci++;

        } else if (strcmp(secim, "3") == 0) {
            // ---- RGB -> HSV ----
            printf("\n  [ACIKLAMA] RGB renk uzayı matematiksel formüllerle HSV (Hue, Saturation, Value) uzayına çevriliyor...\n");
            rgb_to_hsv(img);
            uygulanan_islem = "HSV";
            islem_sayaci++;

        } else if (strcmp(secim, "4") == 0) {
            // ---- HSV -> RGB ----
            printf("\n  [ACIKLAMA] HSV uzayı geri RGB renk uzayına çevriliyor ve taşmalara karşı clamp ediliyor...\n");
            hsv_to_rgb(img);
            clamp_image(img); // Döngüsel kayıplara karşı koruma
            uygulanan_islem = "HSV->RGB";
            islem_sayaci++;

        } else if (strcmp(secim, "5") == 0) {
            printf("\n  [ALT MENÜ] NN Resize Seçildi:\n");
            printf("  1. Büyült (x2)\n");
            printf("  2. Küçült (x0.5)\n");
            printf("  Seçiminiz [1-2]: ");
            char sub[16];
            if (fgets(sub, sizeof(sub), stdin)) {
                if (sub[0] == '1') {
                    printf("\n  [ACIKLAMA] Görüntü En Yakın Komşu (Nearest Neighbor) x2 boyutlandırılıyor...\n");
                    img = nn_resize(img, img.w * 2, img.h * 2);
                    uygulanan_islem = "NN Resize (x2)";
                    islem_sayaci++;
                } else if (sub[0] == '2') {
                    if (img.w < 2 || img.h < 2) {
                        printf("\n  [HATA] Görüntü daha fazla küçültülemez.\n");
                    } else {
                        printf("\n  [ACIKLAMA] Görüntü En Yakın Komşu (Nearest Neighbor) x0.5 boyutlandırılıyor...\n");
                        img = nn_resize(img, img.w / 2, img.h / 2);
                        uygulanan_islem = "NN Resize (x0.5)";
                        islem_sayaci++;
                    }
                } else {
                    printf("  [HATA] Gecersiz alt secim.\n");
                }
            }

        } else if (strcmp(secim, "6") == 0) {
            printf("\n  [ALT MENÜ] Bilinear Resize Seçildi:\n");
            printf("  1. Büyült (x2)\n");
            printf("  2. Küçült (x0.5)\n");
            printf("  Seçiminiz [1-2]: ");
            char sub[16];
            if (fgets(sub, sizeof(sub), stdin)) {
                if (sub[0] == '1') {
                    printf("\n  [ACIKLAMA] Görüntü Çift Doğrusal (Bilinear) x2 boyutlandırılıyor...\n");
                    img = bilinear_resize(img, img.w * 2, img.h * 2);
                    uygulanan_islem = "BL Resize (x2)";
                    islem_sayaci++;
                } else if (sub[0] == '2') {
                    if (img.w < 2 || img.h < 2) {
                        printf("\n  [HATA] Görüntü daha fazla küçültülemez.\n");
                    } else {
                        printf("\n  [ACIKLAMA] Görüntü Çift Doğrusal (Bilinear) x0.5 boyutlandırılıyor...\n");
                        img = bilinear_resize(img, img.w / 2, img.h / 2);
                        uygulanan_islem = "BL Resize (x0.5)";
                        islem_sayaci++;
                    }
                } else {
                    printf("  [HATA] Gecersiz alt secim.\n");
                }
            }

        } else if (strcmp(secim, "7") == 0) {
            printf("\n  [ACIKLAMA] 7x7 boyutunda kutu filtresi üretilip tüm fotağrafa konvolüsyon (blur) uygulanıyor...\n");
            Image filter = make_box_filter(7);
            img = convolve_image(img, filter, 1);
            uygulanan_islem = "Box Blur";
            islem_sayaci++;

        } else if (strcmp(secim, "8") == 0) {
            printf("\n  [ACIKLAMA] Slayttaki Keskinleştirme (Sharpen) matrisi konvolüsyon ile uygulanıyor...\n");
            Image filter = make_sharpen_filter();
            img = convolve_image(img, filter, 1);
            clamp_image(img);
            uygulanan_islem = "Sharpen";
            islem_sayaci++;

        } else if (strcmp(secim, "9") == 0) {
            printf("\n  [ACIKLAMA] Verilen Sigma değerinde mükemmel Gauss dağılımı matrisi türetilip blur yapılıyor...\n");
            Image filter = make_gaussian_filter(2.0f);
            img = convolve_image(img, filter, 1);
            uygulanan_islem = "Gaussian Blur";
            islem_sayaci++;

        } else if (strcmp(secim, "0") == 0) {
            printf("\n  [ACIKLAMA] Gx ve Gy Edge Detection matrisleri bulunup X/Y türevleriyle kenarlar ve açıları bulunuyor...\n");
            img = colorize_sobel(img);
            uygulanan_islem = "Sobel Edge";
            islem_sayaci++;
            
        } else if (strcmp(secim, "p") == 0 || strcmp(secim, "P") == 0) {
            goster_piksel = !goster_piksel;
            printf("\n  [BILGI] Piksel degisimi gosterme durumu: %s\n", goster_piksel ? "ACIK" : "KAPALI");

        } else if (strcmp(secim, "s") == 0 || strcmp(secim, "S") == 0) {
            // ---- KAYDET VE DEVAM ----
            std::string fname = generate_timestamped_filename("sonuc", "jpg");
            if (save_image(img, fname.c_str())) {
                printf("  [BILGI] Fotograf basariyla kaydedildi.\n");
                uygulanan_islem = "Kaydet (s)"; // UX tutarliligi icin ekledik
            } else {
                printf("  [HATA] Kayit islemi basarisiz oldu! 'output' klasoru mevcut mu?\n");
            }
            printf("  [BILGI] Isleme yeni goruntu uzerinden (devam ederek) calisiliyor...\n");

        } else if (strcmp(secim, "r") == 0 || strcmp(secim, "R") == 0) {
            // ---- ORİJİNALE DÖN ----
            printf("\n  [BILGI] Orijinal giris.jpg dosyasi yeniden yukleniyor...\n");
            img = load_image(input_path);
            islem_sayaci = 0;
            islem_gecmisi.clear();

        } else if (strcmp(secim, "q") == 0 || strcmp(secim, "Q") == 0) {
            // ---- KAYDET VE CIK ----
            std::string fname = generate_timestamped_filename("final", "jpg");
            save_image(img, fname.c_str());

            printf("\n");
            printf("  ╔══════════════════════════════════════════════════════════╗\n");
            printf("  ║  PROGRAM SONLANDI                                      ║\n");
            printf("  ╠══════════════════════════════════════════════════════════╣\n");
            printf("  ║  Toplam islem sayisi: %-32d ║\n", islem_sayaci);
            printf("  ║  Cikti dosyasi: %-37s ║\n", fname.c_str());
            printf("  ╚══════════════════════════════════════════════════════════╝\n");
            devam = false;

        } else {
            printf("\n  [UYARI] Gecersiz secim: '%s'\n", secim);
        }

        if (!uygulanan_islem.empty() && islem_sayaci > prev_islem_sayaci) {
            islem_gecmisi.push_back(uygulanan_islem);
            if (goster_piksel && img.data && img.w > 0 && img.h > 0) {
                int cx = img.w / 2;
                int cy = img.h / 2;
                int idx = cy * img.w + cx;
                int wh = img.w * img.h;
                printf("\n  ==== PIKSEL DEGERLERI ==== (Acik)\n");
                printf("  [ONCESI] Merkez (%d,%d) -> ", get_px_w / 2, get_px_h / 2);
                for (int c = 0; c < get_px_c && c < 3; ++c) {
                    printf("%c: %.3f ", (c == 0 ? 'R' : (c == 1 ? 'G' : 'B')), before_p[c]);
                }
                printf("\n  [SONRASI] Merkez (%d,%d) -> ", cx, cy);
                for (int c = 0; c < img.c && c < 3; ++c) {
                    printf("%c: %.3f ", (c == 0 ? 'R' : (c == 1 ? 'G' : 'B')), img.data[c * wh + idx]);
                }
                printf("\n  ==========================\n");
            }

            printf("\n  >> [BASARILI] '%s' islemi goruntuye islendi!\n", uygulanan_islem.c_str());
            printf("  >> Ana menuye donmek icin [ENTER] tusuna basin... ");
            fflush(stdout);
            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        } else if (devam) {
            // Islem sayaci artmadiysa (hata, p basildi, s basildi, r basildi vs.) ama devam ediliyorsa
            printf("\n  >> Devam etmek icin [ENTER] tusuna basin... ");
            fflush(stdout);
            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        }
    }

    // Image yikici (destructor) otomatik olarak bellek temizler
    return 0;
}
