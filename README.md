# ⏱️ ch32v003_timer

Thư viện quản lý thời gian hệ thống không chặn (Non-blocking Timing) mang phong cách Arduino dành cho vi điều khiển **CH32V003** trên nền framework **ch32v003fun**.
Thư viện khai thác trực tiếp bộ đếm SysTick 32-bit phần cứng của lõi RISC-V, cung cấp các hàm định thời gian thực (`millis`, `micros`, `ticks`) và các hàm tính khoảng thời gian chênh lệch (`elapsedMs`, `elapsedUs`) tự động xử lý tràn số (Rollover).

---

## 🚀 Tính năng nổi bật

- **Giao diện thân thuộc phong cách Arduino:** Cung cấp đầy đủ các hàm `millis()` và `micros()` chuẩn xác theo tần số xung nhịp hệ thống (`FUNCONF_SYSTEM_CORE_CLOCK`).
- **Định thời không chặn (Non-blocking):** Thay thế các hàm delay gây nghẽn CPU, hỗ trợ viết lập trình đa nhiệm (Multitasking) và đa tác vụ theo chu kỳ.
- **Tự động xử lý tràn số (Rollover):** Các hàm `elapsedMs()` và `elapsedUs()` sử dụng phép toán bù 2 trên kiểu dữ liệu `uint32_t`, đảm bảo tính toán chênh lệch chính xác 100% ngay cả khi bộ đếm bị tràn.
- **Siêu nhẹ & Tối ưu:** Đọc trực tiếp bộ đếm phần cứng `funSysTick32()` của `ch32fun`, không tốn tài nguyên bộ đếm Timer ngoại vi (TIM1/TIM2).

---

## 📑 Cấu trúc hàm (API Reference)

### 1. Đọc mốc thời gian hệ thống

| Hàm xử lý      | Tham số | Giá trị trả về | Mô tả                                                                               |
| -------------- | ------- | -------------- | ----------------------------------------------------------------------------------- |
| **`ticks()`**  | `void`  | `uint32_t`     | Trả về số tick thô trực tiếp từ bộ đếm SysTick phần cứng của lõi RISC-V.            |
| **`micros()`** | `void`  | `uint32_t`     | Trả về số micro-giây ($\mu\text{s}$) đã trôi qua kể từ khi vi điều khiển khởi động. |
| **`millis()`** | `void`  | `uint32_t`     | Trả về số mili-giây ($\text{ms}$) đã trôi qua kể từ khi vi điều khiển khởi động.    |

### 2. Tính toán khoảng thời gian chênh lệch (Non-blocking)

| Hàm xử lý                       | Tham số                              | Giá trị trả về | Mô tả                                                                                  |
| ------------------------------- | ------------------------------------ | -------------- | -------------------------------------------------------------------------------------- |
| **`elapsedUs(current, start)`** | `uint32_t current`, `uint32_t start` | `uint32_t`     | Tính thời gian trôi qua giữa 2 mốc $\mu\text{s}$ (xử lý chính xác hiện tượng tràn số). |
| **`elapsedMs(current, start)`** | `uint32_t current`, `uint32_t start` | `uint32_t`     | Tính thời gian trôi qua giữa 2 mốc $\text{ms}$ (xử lý chính xác hiện tượng tràn số).   |

---

## 📝 Code mẫu sử dụng

### Chớp tắt LED không chặn (Blink Without Delay) & Quét tác vụ theo chu kỳ

```c
#include "ch32fun.h"
#include <ch32v003_gpio.h>
#include <ch32v003_timer.h>

#define LED_PIN MCU_PIN4

static uint32_t lastBlinkTime = 0;
static uint32_t lastTaskTime  = 0;
static bool ledState          = false;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    uint32_t nowMs = millis();
    uint32_t nowUs = micros();

    // 1. Tác vụ chớp tắt LED mỗi 500ms (Không gây chặn CPU)
    if (elapsedMs(nowMs, lastBlinkTime) >= 500) {
        lastBlinkTime = nowMs;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }

    // 2. Tác vụ nhanh đòi hỏi độ chính xác cao theo Micro-giây (mỗi 1000us = 1ms)
    if (elapsedUs(nowUs, lastTaskTime) >= 1000) {
        lastTaskTime = nowUs;
        // Thực thi công việc định kỳ siêu ngắn tại đây...
    }
}

int main() {
    SystemInit();
    setup();

    while (1) {
        loop();
    }
}

```

---
