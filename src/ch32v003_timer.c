#include "ch32v003_timer.h"

// Số ticks SysTick ứng với 1ms / 1us (tự thích ứng FUNCONF_SYSTICK_USE_HCLK) [ch32fun]
#define TIMER_TICKS_PER_MS DELAY_MS_TIME
#define TIMER_TICKS_PER_US DELAY_US_TIME

static volatile uint32_t systemMillis = 0;

/**
 * @brief Chia 3 chỉ bằng dịch bit + cộng (RV32EC không có lệnh chia), kết quả chính xác (floor).
 */
static inline uint32_t timerDivideBy3(const uint32_t x)
{
    // x/3 ≈ x * (1/4 + 1/16 + 1/64 + ...) luôn hơi nhỏ hơn -> bù lại bằng phần dư
    uint32_t q = (x >> 2) + (x >> 4) + (x >> 6) + (x >> 8) + (x >> 10) + (x >> 12) + (x >> 14) + (x >> 16);
    uint32_t r = x - (q + (q << 1));
    while (r >= 3)
    {
        q++;
        r -= 3;
    }
    return q;
}

/**
 * @brief Quy đổi ticks SysTick sang micro-giây không dùng phép chia phần mềm.
 * @details 48MHz (HCLK) = 3 * 16 ticks/us, HCLK/8 = 3 * 2 ticks/us -> dịch bit rồi chia 3.
 */
static inline uint32_t timerTicksToMicros(const uint32_t tickCount)
{
#if (TIMER_TICKS_PER_US == 48)
    return timerDivideBy3(tickCount >> 4);
#elif (TIMER_TICKS_PER_US == 6)
    return timerDivideBy3(tickCount >> 1);
#else
    return tickCount / TIMER_TICKS_PER_US;
#endif
}

void SysTick_Handler(void) __attribute__((interrupt));

void SysTick_Handler(void)
{
    // Dời mốc so sánh thêm đúng 1ms (không reset CNT) để không trôi thời gian
    SysTick->CMP += TIMER_TICKS_PER_MS;
    SysTick->SR = 0;
    systemMillis++;
}

void timerInit(void)
{
    if (SysTick->CTLR & SYSTICK_CTLR_STIE)
    {
        return; // Đã khởi động trước đó
    }

    // Gieo mốc từ bộ đếm hiện tại để millis() bám sát thời điểm khởi động (đúng trong 89s đầu)
    const uint32_t now = SysTick->CNT;
    systemMillis = now / TIMER_TICKS_PER_MS;
    SysTick->CMP = now - (now % TIMER_TICKS_PER_MS) + TIMER_TICKS_PER_MS;
    SysTick->SR = 0;
    SysTick->CTLR |= SYSTICK_CTLR_STIE;

    NVIC_EnableIRQ(SysTick_IRQn);
    __enable_irq();
}

uint32_t ticks(void)
{
    return funSysTick32();
}

uint32_t micros(void)
{
    timerInit();

    uint32_t ms;
    uint32_t cnt;
    uint32_t cmp;
    do
    {
        ms = systemMillis;
        cnt = SysTick->CNT;
        cmp = SysTick->CMP;
    }
    while (ms != systemMillis); // Ngắt chen ngang giữa chừng -> đọc lại cho nhất quán

    // CMP luôn trỏ tới mốc ms kế tiếp, nên mốc ms hiện tại = CMP - 1ms.
    // ms * 1000 = ms * 1024 - ms * 16 - ms * 8 (chỉ dịch bit, tránh phép nhân phần mềm)
    return (ms << 10) - (ms << 4) - (ms << 3) + timerTicksToMicros(cnt - (cmp - TIMER_TICKS_PER_MS));
}

uint32_t millis(void)
{
    timerInit();
    return systemMillis;
}

uint32_t elapsedUs(const uint32_t current, const uint32_t start)
{
    return current - start;
}

uint32_t elapsedMs(const uint32_t current, const uint32_t start)
{
    return current - start;
}
