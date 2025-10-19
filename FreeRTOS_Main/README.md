# 🧠 Demo FreeRTOS – Giao tiếp giữa 2 Task bằng Queue

## 📄 Mô tả chương trình
Chương trình demo sử dụng **FreeRTOS** trên **STM32F1 (HAL)**.  
Có 2 task và 1 queue để truyền dữ liệu:

- **Task03**: gửi thông số tần số (`frequency`) và độ rộng xung (`duty`) vào queue mỗi 3 giây.  
- **Task02**: nhận thông số từ queue và điều khiển nháy LED theo các giá trị đó.

Cấu trúc giúp minh họa cách 2 task trao đổi dữ liệu thông qua **Queue** trong FreeRTOS.

---

## ⚙️ Cấu trúc dữ liệu truyền qua Queue
```c
typedef struct {
  uint32_t frequency;  // Hz
  uint8_t duty;        // %
} BlinkParam_t;
Mỗi phần tử trong queue là một struct BlinkParam_t, chứa:

frequency: tần số nháy LED (Hz)

duty: phần trăm thời gian LED bật (duty cycle)

🧩 Phần khởi tạo chính – main()
c
Sao chép mã
int main(void)
{
  HAL_Init();                  // Khởi tạo thư viện HAL
  SystemClock_Config();        // Cấu hình xung nhịp hệ thống
  MX_GPIO_Init();              // Khởi tạo GPIO dùng cho LED

  // Tạo Queue có 5 phần tử kiểu BlinkParam_t
  blinkQueueHandle = xQueueCreate(5, sizeof(BlinkParam_t));
  if (blinkQueueHandle == NULL)
  {
    Error_Handler();           // Nếu tạo queue thất bại
  }

  // Tạo các task
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  osThreadDef(myTask03, StartTask03, osPriorityLow, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  osKernelStart();             // Bắt đầu chạy Scheduler của FreeRTOS

  while (1) {}                 // Không bao giờ đến đây
}
Giải thích:

xQueueCreate(5, sizeof(BlinkParam_t)): tạo một hàng đợi có thể chứa 5 phần tử kiểu BlinkParam_t.

osThreadDef() + osThreadCreate(): đăng ký và khởi tạo các task.

osKernelStart(): bắt đầu hoạt động đa nhiệm.

💡 Task mặc định – StartDefaultTask
c
Sao chép mã
void StartDefaultTask(void const * argument)
{
  for(;;)
  {
    osDelay(1);   // Task rảnh, chỉ chờ 1 tick rồi lặp lại
  }
}
Đây là task mặc định do CubeMX sinh ra, không làm gì ngoài việc delay.

🔁 Task02 – Nhận dữ liệu từ Queue & điều khiển LED
c
Sao chép mã
void StartTask02(void const * argument)
{
  BlinkParam_t param = {1, 50}; // Mặc định 1Hz, duty 50%

  for(;;)
  {
    // Nếu có dữ liệu mới từ queue thì cập nhật thông số
    if (xQueueReceive(blinkQueueHandle, &param, 0) == pdPASS)
    {
      // Cập nhật tần số và duty mới
    }

    uint32_t period = 1000 / param.frequency; // chu kỳ (ms)
    uint32_t onTime = (period * param.duty) / 100;
    uint32_t offTime = period - onTime;

    // Bật LED
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    osDelay(onTime);

    // Tắt LED
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    osDelay(offTime);
  }
}
Giải thích chi tiết:

xQueueReceive(): nhận phần tử từ queue (nếu có).

Nếu có dữ liệu mới thì thay đổi param.frequency và param.duty.

period, onTime, offTime tính toán dựa trên tần số và duty.

LED bật trong onTime rồi tắt trong offTime, tạo hiệu ứng PWM bằng phần mềm.

📤 Task03 – Gửi dữ liệu mới vào Queue
c
Sao chép mã
void StartTask03(void const * argument)
{
  BlinkParam_t param;
  uint32_t freqs[] = {1, 2, 4, 5}; // các tần số (Hz)
  uint8_t dutys[] = {20, 50, 80};  // các duty (%)
  uint8_t i = 0, j = 0;

  for(;;)
  {
    param.frequency = freqs[i];
    param.duty = dutys[j];

    // Gửi cấu trúc vào queue
    xQueueSend(blinkQueueHandle, &param, 0);

    i = (i + 1) % 4; // đổi tần số
    j = (j + 1) % 3; // đổi duty
    osDelay(3000);   // gửi mới mỗi 3 giây
  }
}
Giải thích chi tiết:

Task03 tuần tự chọn tần số và duty từ mảng freqs[], dutys[].

Sau mỗi 3 giây, task gửi 1 giá trị mới vào queue.

Task02 nhận được và điều chỉnh LED tương ứng.

⚙️ Cấu hình Clock
c
Sao chép mã
void SystemClock_Config(void)
Hàm này được CubeMX tự sinh, thiết lập hệ thống chạy với:

Nguồn HSE (ngoại vi)

PLL nhân 9 → SYSCLK = 72 MHz

Các bus APB1, APB2 được chia phù hợp cho STM32F103.

🔌 Khởi tạo GPIO cho LED
c
Sao chép mã
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}
Cấu hình LED tại chân PA0, chế độ Output Push-Pull, tốc độ thấp.

🧯 Hàm xử lý lỗi
c
Sao chép mã
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    HAL_Delay(200);
  }
}
Khi lỗi xảy ra (ví dụ không tạo được queue), LED sẽ nhấp nháy nhanh để báo hiệu
