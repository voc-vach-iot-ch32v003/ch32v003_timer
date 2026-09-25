# ⏱️ ch32v003_timer

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Supported-orange.svg)
![Framework](https://img.shields.io/badge/Framework-ch32v003fun-green.svg)
![MCU](https://img.shields.io/badge/MCU-CH32V003-red.svg)

Thư viện quản lý thời gian hệ thống **không chặn (Non-blocking)** cho vi điều khiển CH32V003, dựa trên bộ đếm SysTick 32-bit phần cứng của lõi RISC-V.

---

## 🚀 Tính năng nổi bật

- Đọc thời gian hệ thống theo micro-giây (`micros()`) và mili-giây (`millis()`) kể từ lúc khởi động.
- Tính khoảng thời gian trôi qua (`elapsedUs()`, `elapsedMs()`) chuẩn xác, tự động xử lý hiện tượng tràn số (rollover) của bộ đếm 32-bit.
- Truy cập trực tiếp giá trị tick thô (`ticks()`) khi cần độ phân giải cao nhất.
- Không phụ thuộc ngoại vi Timer riêng, tận dụng SysTick sẵn có của `ch32fun` → không tốn thêm tài nguyên phần cứng.

---

## 📦 Cài đặt

Thêm dòng sau vào `platformio.ini` của dự án:

```ini
lib_deps =
    https://github.com/voc-vach-iot-ch32v003/ch32v003_timer.git
```

---

## ⚙️ Cấu hình môi trường

Thư viện sử dụng trực tiếp `FUNCONF_SYSTEM_CORE_CLOCK` từ `ch32fun`, không yêu cầu build flag riêng. Đảm bảo project đã cấu hình đúng xung nhịp hệ thống trong `funconfig.h`:

```c
// funconfig.h
#define FUNCONF_SYSTICK_USE_HCLK 1
```

---

## 📝 Code mẫu sử dụng (Quick Start)

Ví dụ chớp LED không chặn (non-blocking blink) bằng `millis()` và `elapsedMs()`:

```c
#include "ch32fun.h"
#include "ch32v003_timer.h"

#define LED_PIN PD6

static uint32_t lastToggle = 0;
static uint8_t ledState = 0;

int main(void)
{
    SystemInit();

    funGpioInitAll();
    funPinMode(LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP);

    while (1)
    {
        uint32_t now = millis();

        // Chớp LED mỗi 500ms mà KHÔNG dùng delay() chặn chương trình
        if (elapsedMs(now, lastToggle) >= 500)
        {
            lastToggle = now;
            ledState = !ledState;
            funDigitalWrite(LED_PIN, ledState);
        }

        // Các tác vụ khác vẫn chạy song song tại đây...
    }
}
```

---

## 📑 Tra cứu API (API Reference)

### Nhóm: Đọc thời gian hệ thống

| Hàm xử lý  | Tham số | Giá trị trả về | Mô tả                                               |
| :--------- | :------ | :------------- | :-------------------------------------------------- |
| `ticks()`  | Không   | `uint32_t`     | Số tick thô từ bộ đếm SysTick phần cứng.            |
| `micros()` | Không   | `uint32_t`     | Số micro-giây (us) đã trôi qua kể từ lúc khởi động. |
| `millis()` | Không   | `uint32_t`     | Số mili-giây (ms) đã trôi qua kể từ lúc khởi động.  |

### Nhóm: Tính khoảng thời gian trôi qua

| Hàm xử lý     | Tham số                                                         | Giá trị trả về | Mô tả                                                            |
| :------------ | :-------------------------------------------------------------- | :------------- | :--------------------------------------------------------------- |
| `elapsedUs()` | `current`: mốc hiện tại (từ `micros()`)<br>`start`: mốc bắt đầu | `uint32_t`     | Khoảng chênh lệch (us) giữa hai mốc thời gian, tự xử lý tràn số. |
| `elapsedMs()` | `current`: mốc hiện tại (từ `millis()`)<br>`start`: mốc bắt đầu | `uint32_t`     | Khoảng chênh lệch (ms) giữa hai mốc thời gian, tự xử lý tràn số. |

---

## 📄 Giấy phép & Tác giả

- **Tác giả:** [Vọc Vạch IoT](https://github.com/voc-vach-iot)
- **Giấy phép:** Phát hành theo giấy phép [MIT License](LICENSE).
