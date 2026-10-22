# ⏱️ ch32v003_timer

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Supported-orange.svg)
![Framework](https://img.shields.io/badge/Framework-ch32v003fun-green.svg)
![MCU](https://img.shields.io/badge/MCU-CH32V003-red.svg)

Thư viện đo thời gian hệ thống **không chặn (Non-blocking)** kiểu Arduino (`millis()`, `micros()`) cho vi điều khiển CH32V003, dựa trên bộ đếm SysTick của lõi RISC-V.

---

## 🚀 Tính năng nổi bật

- **`millis()` / `micros()` không bị quay về 0 sau ~89 giây:** bộ đếm mili-giây 32-bit được tăng bởi ngắt so sánh SysTick mỗi 1ms, chỉ tràn sau **49.7 ngày** (`millis`) / **71.6 phút** (`micros`) giống Arduino.
- **Tự khởi động:** lần gọi `millis()`/`micros()` đầu tiên tự bật bộ đếm; gọi `timerInit()` sớm trong `setup()` nếu muốn mốc thời gian bám sát lúc khởi động.
- **`micros()` nhẹ:** không dùng phép nhân/chia phần mềm (CH32V003 là RV32EC, không có lệnh nhân/chia) — khoảng **180 chu kỳ máy (~3.7µs)** mỗi lần gọi.
- **So sánh mốc thời gian an toàn khi tràn số** với `elapsedMs()` / `elapsedUs()`.
- **Không ảnh hưởng `delayMs()` / `delayUs()`:** bộ đếm SysTick vẫn chạy tự do, chỉ dùng thêm thanh ghi so sánh.

> ⚠️ Thư viện chiếm trình phục vụ ngắt `SysTick_Handler`. Không định nghĩa lại hàm này trong dự án.

---

## 📦 Cài đặt (Installation)

```ini
lib_deps =
    https://github.com/voc-vach-iot-ch32v003/ch32v003_timer.git
```

---

## ⚙️ Cấu hình môi trường

Không cần build flag riêng. Thư viện tự thích ứng nguồn clock SysTick theo `funconfig.h`:

```c
// funconfig.h — khuyến nghị: SysTick chạy ở HCLK 48MHz để độ phân giải micros() tốt nhất
#define FUNCONF_SYSTICK_USE_HCLK 1
```

Độ chính xác tuyệt đối phụ thuộc dao động nội HSI của chip (sai số nhà sản xuất cho phép, đo được -1.0% … +0.25% trên bàn test).

---

## 📝 Code mẫu sử dụng (Quick Start)

Chớp LED không chặn mỗi 500ms:

```c
#include "ch32fun.h"
#include "ch32v003_delay.h"
#include "ch32v003_gpio.h"
#include "ch32v003_timer.h"

#define LED_PIN MCU_PIN3 // Chân vật lý số 3 (J4M6: PA2, A4M6: PC3, F4P6: PD6)

static uint32_t lastToggleMs = 0;

static void setup(void)
{
    pinMode(LED_PIN, OUTPUT);
    timerInit(); // Không bắt buộc: millis() tự khởi động ở lần gọi đầu
}

static void loop(void)
{
    const uint32_t now = millis();
    if (elapsedMs(now, lastToggleMs) >= 500)
    {
        lastToggleMs = now;
        digitalToggle(LED_PIN);
    }
    // Các tác vụ khác vẫn chạy song song tại đây...
}

int main(void)
{
    SystemInit();
    delayMs(1000); // Khoảng chờ an toàn tránh brick chip

    setup();
    while (1)
    {
        loop();
    }
}
```

---

## 📁 Danh sách Ví dụ mẫu (Examples)

Thư viện tiện ích lõi: không có thư mục `examples/`, cách dùng được trình bày ở Quick Start.

---

## 📑 Tra cứu API (API Reference)

### Khởi động & đọc thời gian

| Hàm xử lý | Tham số | Giá trị trả về | Mô tả |
| :-- | :-- | :-- | :-- |
| `timerInit()` | Không | `void` | Bật ngắt SysTick 1ms để đếm mili-giây. Gọi nhiều lần không sao; `millis()`/`micros()` tự gọi nếu chưa bật. |
| `millis()` | Không | `uint32_t` | Số mili-giây kể từ khi bộ đếm khởi động. Tràn sau ~49.7 ngày. |
| `micros()` | Không | `uint32_t` | Số micro-giây kể từ khi bộ đếm khởi động. Tràn sau ~71.6 phút. |
| `ticks()` | Không | `uint32_t` | Giá trị thô của SysTick (48MHz khi `FUNCONF_SYSTICK_USE_HCLK = 1`). Tràn sau ~89 giây, chỉ dùng đo khoảng ngắn. |

### Khoảng thời gian trôi qua

| Hàm xử lý | Tham số | Giá trị trả về | Mô tả |
| :-- | :-- | :-- | :-- |
| `elapsedMs(current, start)` | `uint32_t current`, `uint32_t start` | `uint32_t` | `current - start` (ms), đúng cả khi bộ đếm đã tràn. |
| `elapsedUs(current, start)` | `uint32_t current`, `uint32_t start` | `uint32_t` | `current - start` (us), đúng cả khi bộ đếm đã tràn. |

---

## 📄 Giấy phép (License) & Tác giả

- **Tác giả:** [Vọc Vạch IoT](https://github.com/voc-vach-iot)
- **Giấy phép:** Phát hành theo giấy phép [MIT License](LICENSE).
