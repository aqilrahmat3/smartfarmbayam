# Smart Farm Sensor Simulation (Wokwi)

## Deskripsi Proyek
Proyek ini merupakan simulasi sistem **Smart Farming** berbasis **ESP32** yang dijalankan menggunakan **Wokwi** di **Visual Studio Code**. Sistem ini mensimulasikan pembacaan **empat sensor utama**, yaitu:

- **pH**
- **Suhu (Temperature)**
- **TDS**
- **EC**

Data sensor digunakan untuk memantau kondisi tanaman dan dikirimkan ke **Firebase Realtime Database**. Proyek ini mendukung dua mode operasi, yaitu **Manual** dan **Auto**, serta dilengkapi dengan sistem **alert** ke Firebase.

Proyek ini dibuat sebagai contoh implementasi monitoring dan kontrol dasar pada sistem pertanian pintar.

---

## Fitur Utama
- Simulasi pembacaan sensor pH, Suhu, TDS, dan EC
- Pengiriman data sensor ke Firebase Realtime Database
- Mode operasi **Manual** dan **Auto**
- Sistem alert ke Firebase jika nilai sensor melewati batas
- Kontrol pompa nutrisi dan pompa pH (disimulasikan menggunakan lampu)

---

## Mode Operasi

### Mode Auto
- Sistem membandingkan nilai sensor dengan batas yang ditentukan
- Pompa nutrisi dan pompa pH dikontrol otomatis
- Logika kontrol berjalan sepenuhnya di dalam kode
- Pada Wokwi, pompa direpresentasikan menggunakan **lampu**

### Mode Manual
- Tidak ada logika otomatis
- Pompa nutrisi dan pompa pH dikendalikan manual
- Lampu hanya menyala atau mati sesuai perintah

---

## Alert System
Sistem akan mengirimkan **alert ke Firebase** apabila:
- Nilai sensor berada di luar batas aman
- Kondisi tertentu terpenuhi sesuai logika program

---

## Prasyarat
Sebelum menjalankan proyek ini, pastikan sudah terpasang di **Visual Studio Code**:

- **PlatformIO Extension**
- **Wokwi Extension**

Jika kedua extension tersebut belum terpasang, proyek **tidak bisa dijalankan**.

---

## Cara Menjalankan Proyek

### 1. Clone Repository
```bash
git clone https://github.com/username/nama-repo.git
cd nama-repo
````

---

### 2. Buat File `env.h`

Setelah clone repository, **WAJIB** membuat file konfigurasi Firebase.

1. Masuk ke folder:

   ```
   src/
   ```
2. Salin file contoh:

   ```bash
   cp env_example.h env.h
   ```
3. Isi file `env.h` sesuai konfigurasi Firebase:

   ```cpp
   #define FIREBASE_HOST "your-project-id.firebaseio.com"
   #define FIREBASE_EMAIL "your-email@gmail.com"
   #define FIREBASE_PASSWORD "your-password"
   #define FIREBASE_API_KEY "your-api-key"
   ```

> **Catatan:**
> File `env.h` berisi data sensitif dan **tidak boleh di-commit ke repository**.

---

### 3. Menjalankan Simulasi (PENTING)

1. Buka proyek menggunakan **Visual Studio Code**
2. Pastikan **PlatformIO** dan **Wokwi Extension** sudah aktif
3. Build projek dengan Platform io terlebih dahulu
4. Buka file:

   ```
   diagram.json
   ```
5. Klik **ikon ▶️ Play (Wokwi Play Button)**
---

## Catatan Tambahan

* Proyek ini sepenuhnya berbasis **simulasi**
* Lampu digunakan sebagai representasi pompa nutrisi dan pompa pH
* Logika batas sensor dapat disesuaikan di dalam kode
* Aplikasi mobile yang digunakan : https://github.com/PurnamaRidzkyN/smart_farm_bayam 
---


