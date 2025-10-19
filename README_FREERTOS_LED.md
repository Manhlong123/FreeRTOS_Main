# 🧠 FreeRTOS LED Blink Demo – STM32F103C8T6 (Blue Pill)

## 1️⃣ Mục đích
Dự án minh họa cách **truyền dữ liệu giữa các task trong FreeRTOS** bằng **hàng đợi (Queue)**.  
Hai task sẽ phối hợp:
- **Task03** gửi dữ liệu (tần số & duty) vào queue.
- **Task02** nhận dữ liệu và điều khiển LED nhấp nháy tương ứng.

---

## 2️⃣ Cấu trúc dữ liệu chính
```c
typedef struct {
  uint32_t frequency;  // Tần số nhấp nháy (Hz)
  uint8_t duty;        // Chu kỳ hoạt động (%)
} BlinkParam_t;
```

👉 Mỗi phần tử trong queue là một `BlinkParam_t`, chứa:
- `frequency`: số lần LED bật/tắt mỗi giây  
- `duty`: tỉ lệ phần trăm thời gian LED bật trong một chu kỳ

---

## 3️⃣ Cách hoạt động tổng thể

### ⚙️ a. Khởi tạo
Trong `main()`:
1. Gọi `HAL_Init()` và cấu hình clock 72 MHz.  
2. Khởi tạo GPIO cho LED (chân **PA0**).  
3. Tạo queue `blinkQueueHandle` với sức chứa **5 phần tử** (`BlinkParam_t`).  
4. Tạo và khởi động 3 task:
   - `defaultTask` (rảnh rỗi, không làm gì)
   - `StartTask02` (điều khiển LED)
   - `StartTask03` (gửi dữ liệu vào queue)

Cuối cùng gọi `osKernelStart()` để bắt đầu scheduler của FreeRTOS.

---

### 💡 b. Task03 – Gửi dữ liệu vào Queue
```c
void StartTask03(void const * argument)
{
  BlinkParam_t param;
  uint32_t freqs[] = {1, 2, 4, 5};
  uint8_t dutys[] = {20, 50, 80};
  uint8_t i = 0, j = 0;

  for(;;)
  {
    param.frequency = freqs[i];
    param.duty = dutys[j];

    xQueueSend(blinkQueueHandle, &param, 0);

    i = (i + 1) % 4;
    j = (j + 1) % 3;

    osDelay(3000);
  }
}
```

#### 🔍 Giải thích:
- Mỗi **3 giây**, Task03 gửi một cấu trúc `param` chứa `frequency` và `duty` vào hàng đợi.  
- Các giá trị tuần tự luân phiên theo mảng `freqs` và `dutys`.  
- `xQueueSend()` thêm phần tử vào queue mà không chờ (`timeout = 0`).

➡️ **Kết quả:** mỗi 3 giây, LED sẽ đổi tần số và duty.

---

### 💡 c. Task02 – Nhận dữ liệu và điều khiển LED
```c
void StartTask02(void const * argument)
{
  BlinkParam_t param = {1, 50};

  for(;;)
  {
    if (xQueueReceive(blinkQueueHandle, &param, 0) == pdPASS)
    {
      // Có dữ liệu mới thì cập nhật giá trị frequency và duty
    }

    uint32_t period = 1000 / param.frequency; // ms
    uint32_t onTime = (period * param.duty) / 100;
    uint32_t offTime = period - onTime;

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    osDelay(onTime);

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    osDelay(offTime);
  }
}
```

#### 🔍 Giải thích:
- Ban đầu LED nhấp nháy 1Hz – duty 50%.
- Task liên tục kiểm tra queue bằng `xQueueReceive()`.
- Nếu có dữ liệu mới: cập nhật `param.frequency` và `param.duty`.
- Sau đó tính toán:
  - `period = 1000 / frequency` → chu kỳ nhấp nháy (ms)
  - `onTime = period * duty / 100` → thời gian LED bật
  - `offTime = period - onTime` → thời gian LED tắt  
- Cuối cùng bật LED, delay `onTime`, tắt LED, delay `offTime`.

➡️ **Kết quả:** LED thay đổi tốc độ và độ sáng tương đối theo duty.

---

### 💡 d. Task mặc định
```c
void StartDefaultTask(void const * argument)
{
  for(;;)
  {
    osDelay(1);
  }
}
```
- Task này chỉ để “chiếm chỗ” cho hệ thống.  
- Giúp scheduler của FreeRTOS có một task IDLE nếu các task khác đang chờ.

---

## 4️⃣ FreeRTOS Queue
Queue được tạo bằng:
```c
blinkQueueHandle = xQueueCreate(5, sizeof(BlinkParam_t));
```
- Kích thước: 5 phần tử  
- Mỗi phần tử: kiểu `BlinkParam_t` (8 byte)  
- Được dùng để **truyền dữ liệu giữa Task03 (Producer)** và **Task02 (Consumer)** mà không cần biến toàn cục.

---

## 5️⃣ Clock & GPIO
```c
SystemClock_Config();  // 72 MHz từ HSE 8 MHz ×9
MX_GPIO_Init();        // Bật clock GPIOA, cấu hình PA0 output push-pull
```
LED ở **PA0** được cấu hình `Output Push-Pull`, tốc độ thấp.  
Hàm `HAL_GPIO_WritePin()` được dùng để bật/tắt LED.

---

## 6️⃣ Kết quả quan sát
- LED ở PA0 nhấp nháy.
- Chu kỳ thay đổi mỗi **3 giây** theo tần số & duty mới:
  | Lần | Frequency (Hz) | Duty (%) |
  |------|----------------|-----------|
  | 1 | 1 | 20 |
  | 2 | 2 | 50 |
  | 3 | 4 | 80 |
  | 4 | 5 | 20 |
  | 5 | (vòng lại) | ... |

---

## 7️⃣ Tổng kết
- Dự án này giúp hiểu cách **task giao tiếp qua queue** trong FreeRTOS.  
- Mô hình phù hợp để mở rộng cho các bài lab như:
  - Gửi dữ liệu cảm biến qua queue.
  - Truyền cấu hình PWM/ADC giữa các task.
  - Hiển thị thông tin qua UART hoặc LCD.

---

## 📚 Kiến thức FreeRTOS minh họa
| Thành phần | Hàm sử dụng | Vai trò |
|-------------|-------------|----------|
| Task | `osThreadDef`, `osThreadCreate` | Tạo 3 task |
| Queue | `xQueueCreate`, `xQueueSend`, `xQueueReceive` | Giao tiếp giữa các task |
| Delay | `osDelay()` | Tạo thời gian thực theo tick |
| Scheduler | `osKernelStart()` | Quản lý luồng thực thi |

---

🧩 **Kết luận:**  
Dự án nhỏ nhưng minh họa đầy đủ khái niệm cốt lõi của FreeRTOS:
- **Task song song**
- **Queue truyền dữ liệu**
- **Scheduler điều phối thời gian**

Rất thích hợp để học FreeRTOS cơ bản trên STM32.
