# 🧠 FreeRTOS Demo: 2 Task + Queue (Blink LED)

## 📘 Giới thiệu
Dự án này minh họa cách sử dụng **FreeRTOS** trên vi điều khiển **STM32F1** để giao tiếp giữa hai **Task** thông qua **Queue**.
Mục tiêu:
- Gửi và nhận cấu trúc dữ liệu (frequency, duty) qua Queue.
- Điều khiển LED nhấp nháy theo tần số và chu kỳ xung được nhận.

## ⚙️ Cấu hình phần cứng
| Thành phần | Chức năng | Ghi chú |
|-------------|------------|---------|
| STM32F103C8T6 | Vi điều khiển chính | Chạy FreeRTOS |
| LED | Hiển thị nhấp nháy | Nối vào `PA0` |
| Clock | HSE 8 MHz | PLL x9 → SYSCLK = 72 MHz |

**Kết nối LED:**
```
PA0 ----> LED ----> GND
```

## 🔁 Nguyên lý hoạt động
### 🧵 Task 03 – Gửi thông số
- Gửi cặp giá trị `frequency` và `duty` sang Queue mỗi **3 giây**.

### 💡 Task 02 – Nhận và điều khiển LED
- Nhận thông số từ Queue.
- LED nhấp nháy theo tần số và duty nhận được.

## 🚀 Cách build & chạy
1. Mở project trong STM32CubeIDE hoặc Keil.
2. Cấu hình clock: HSE 8 MHz, PLL x9 → SYSCLK 72 MHz.
3. Build & Flash xuống board.
4. Quan sát LED PA0: thay đổi tốc độ và duty mỗi 3 giây.

## 💡 Kết quả mong đợi
| Tần số (Hz) | Duty (%) | Mô tả |
|--------------|-----------|-------|
| 1 | 20 | Nhấp chậm, sáng ngắn |
| 2 | 50 | Trung bình |
| 4 | 80 | Nhấp nhanh, sáng lâu |
| 5 | 20 | Rất nhanh, sáng ngắn |

## 📄 Thông tin
**Tác giả:** Mạnh Long  
**Dự án:** Demo FreeRTOS – Task, Queue & LED Control  
**Version:** 1.0
