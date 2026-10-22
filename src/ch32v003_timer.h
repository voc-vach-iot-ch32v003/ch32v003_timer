/**
 * @file ch32v003_timer.h
 * @author Vọc Vạch IoT
 * @brief Thư viện quản lý thời gian hệ thống không chặn (Non-blocking) cho CH32V003.
 */

#ifndef CH32V003_TIMER_H
#define CH32V003_TIMER_H

#include "ch32fun.h"

// ============================================================================
// BỘ HÀM QUẢN LÝ THỜI GIAN HỆ THỐNG (NON-BLOCKING TIMING)
// ============================================================================

/**
 * @brief Khởi động bộ đếm mili-giây nền bằng ngắt so sánh SysTick (1 ngắt / 1ms).
 * @details Không bắt buộc gọi: millis()/micros() tự khởi động ở lần gọi đầu tiên.
 *          Gọi sớm trong setup() để mốc thời gian bám sát thời điểm khởi động.
 *          Bộ đếm SysTick vẫn chạy tự do nên delayMs()/delayUs() không bị ảnh hưởng.
 * @note Thư viện chiếm trình phục vụ ngắt SysTick_Handler.
 */
void timerInit(void);

/**
 * @brief Trả về số ticks thô từ bộ đếm SysTick phần cứng 32-bit của lõi RISC-V.
 * @details Tần số đếm = HCLK (48MHz) khi FUNCONF_SYSTICK_USE_HCLK = 1, ngược lại HCLK/8.
 *          Bộ đếm tràn sau ~89 giây (48MHz) nên chỉ dùng để đo khoảng ngắn.
 * @return Số ticks thô hiện tại.
 */
uint32_t ticks(void);

/**
 * @brief Trả về số micro-giây (us) đã trôi qua kể từ khi khởi động bộ đếm.
 * @details Tràn số sau ~71.6 phút (2^32 us), dùng elapsedUs() để so sánh an toàn.
 * @return Số micro-giây (us).
 */
uint32_t micros(void);

/**
 * @brief Trả về số mili-giây (ms) đã trôi qua kể từ khi khởi động bộ đếm.
 * @details Tràn số sau ~49.7 ngày (2^32 ms), dùng elapsedMs() để so sánh an toàn.
 * @return Số mili-giây (ms).
 */
uint32_t millis(void);

/**
 * @brief Tính toán khoảng thời gian (us) thực tế đã trôi qua giữa hai mốc thời gian micros.
 * @details Tự động xử lý chính xác hiện tượng tràn số (Rollover) của bộ đếm 32-bit.
 * @param current Mốc thời gian hiện tại (thường lấy từ hàm micros()).
 * @param start Mốc thời gian bắt đầu được lưu lại trước đó.
 * @return Khoảng thời gian chênh lệch tính bằng micro-giây (us).
 */
uint32_t elapsedUs(uint32_t current, uint32_t start);

/**
 * @brief Tính toán khoảng thời gian (ms) thực tế đã trôi qua giữa hai mốc thời gian millis.
 * @details Tự động xử lý chính xác hiện tượng tràn số (Rollover) của bộ đếm 32-bit.
 * @param current Mốc thời gian hiện tại (thường lấy từ hàm millis()).
 * @param start Mốc thời gian bắt đầu được lưu lại trước đó.
 * @return Khoảng thời gian chênh lệch tính bằng mili-giây (ms).
 */
uint32_t elapsedMs(uint32_t current, uint32_t start);

#endif // CH32V003_TIMER_H
