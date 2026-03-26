**Proje Github Linki:** [github.com/talhakeskinn/digital-image-processing-cpp](https://github.com/talhakeskinn/digital-image-processing-cpp)

---

## 📷 Proje Hakkında
Bu proje, görüntü işleme dünyasına giriş yapmak ve temel algoritmaların arka planda nasıl çalıştığını anlamak için geliştirilmiş, **harici hiçbir kütüphane (OpenCV vb.) bağımlılığı olmayan** profesyonel bir terminal uygulamasıdır. 

Tüm işlemler, piksellerin bellekte **CHW (Planar - Renk Kanalları Ayrı)** düzeninde tutulması esasına dayanarak, tamamen C++ dilinin gücüyle sıfırdan implement edilmiştir. Görüntü okuma ve yazma işlemleri için sadece hafif siklet `stb_image` kütüphaneleri kullanılmıştır.

## 🚀 Neler Yapabilirsiniz?
Uygulama, karmaşık matematiksel işlemleri kullanıcı dostu bir menü ile sunar:

*   **Akıllı Boyutlandırma (Resize):** Hem **En Yakın Komşu (Nearest Neighbor)** hem de **Çift Doğrusal (Bilinear)** interpolasyon yöntemleriyle görüntüyü 2 kat büyütebilir veya yarısına (0.5x) düşürebilirsiniz.
*   **Renk Uzayı Dönüşümleri:** RGB'den Gri Tona (Luminance) geçişin yanı sıra, HSV (Hue, Saturation, Value) uzayına tam dönüşüm ve geri kazanım işlemleri.
*   **Uzaysal Filtreler (Filtreleme):** Görüntüye bulanıklık (Box & Gaussian Blur), keskinleştirme (Sharpen) veya kabartma (Emboss) gibi konvolüsyon temelli efektler uygulayabilirsiniz.
*   **Kenar Tespiti:** Sobel operatörü ile piksellerin türevlerini hesaplayarak kenarları ve yönlerini tespit edebilirsiniz.
*   **Katmanlı İşlem Takibi:** Yaptığınız her işlem bir öncekinin üzerine eklenir. `s` tuşu ile dilediğiniz aşamada sonucu kaydedebilir, `r` tuşu ile her şeyi sıfırlayıp başa dönebilirsiniz.

## 🛠️ Kurulum ve Çalıştırma

Projeyi çalıştırmak için sisteminizde bir C++ derleyicisinin (g++, clang veya msvc) olması yeterlidir.

### Windows Kullanıcıları
Kök dizindeki **`run.bat`** dosyasına çift tıklamanız. Script sizin için klasörleri oluşturur, derlemeyi yapar ve uygulamayı en yüksek performans optimizasyonuyla başatır.

### Linux ve macOS Kullanıcıları
Terminalden şu komutları sırasıyla çalıştırabilirsiniz:
```bash
chmod +x run.sh
./run.sh
```
*(Manuel derlemek isterseniz: `g++ src/main.cpp -o app -O3 -Wall -I external && ./app`)*

## 💡 Kullanım İpuçları
*   **`input/giris.jpg`:** Uygulama bu dosyayı otomatik arar. Başlamadan önce orada bir resim olduğundan emin olun.
*   **Alt Menüler:** Boyutlandırma (5 veya 6) gibi seçenekleri seçtiğinizde, büyütme mi yoksa küçültme mi yapacağınızı soran bir alt menü ile karşılaşırsınız.
*   **Piksel İzleme (p):** Bu özelliği açtığınızda, her işlemden sonra görüntünün tam merkezindeki pikselin değerlerinin nasıl değiştiğini terminalden canlı olarak takip edebilirsiniz.
*   **Çıktılar:** Kaydettiğiniz her resim, `output/` klasörüne o anki tarih ve saati içeren bir isimle (örn: `sonuc_20260326_123000.jpg`) eklenir.

---
**Geliştirici:** Talha Keskin  
*Bu proje, görüntü işlemenin matematiksel temellerini öğrenmek isteyenler için bir açık kaynak referansı niteliğindedir.*


