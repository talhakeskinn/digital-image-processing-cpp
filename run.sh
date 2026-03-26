#!/bin/bash

# ===================================================
# Sayisal Goruntu Isleme - Tek Tik Calistirici (Linux/Mac)
# ===================================================

# 1. Klasorlerin varligindan emin ol
mkdir -p input output

# 2. Derleme islemi
echo "[BILGI] Derleme baslatiliyor..."
g++ src/main.cpp -o app -O3 -Wall -I external

# 3. Hata kontrolu ve calistirma
if [ $? -eq 0 ]; then
    echo "[TAMAM] Derleme basarili. Uygulama baslatiliyor..."
    echo "---------------------------------------------------"
    chmod +x app
    ./app
else
    echo "[HATA] Derleme sirasinda bir hata olustu. Lutfen g++ yuklu oldugundan emin olun."
    exit 1
fi
