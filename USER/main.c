#include "main.h"
#include "cmsis_os.h"
#include <stdlib.h>

typedef struct {
  uint32_t frequency;  // Hz
  uint8_t duty;        // %
} BlinkParam_t;

#define LED_PIN GPIO_PIN_0
#define LED_PORT GPIOA

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
QueueHandle_t blinkQueueHandle;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  blinkQueueHandle = xQueueCreate(5, sizeof(BlinkParam_t));
  if (blinkQueueHandle == NULL)
    Error_Handler();

  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  osThreadDef(myTask03, StartTask03, osPriorityLow, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  osKernelStart();

  while (1) {}
}

void StartDefaultTask(void const * argument)
{
  for(;;) osDelay(1);
}

void StartTask02(void const * argument)
{
  BlinkParam_t param = {1, 50};

  for(;;)
  {
    if (xQueueReceive(blinkQueueHandle, &param, 0) == pdPASS) {}

    uint32_t period = 1000 / param.frequency;
    uint32_t onTime = (period * param.duty) / 100;
    uint32_t offTime = period - onTime;

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    osDelay(onTime);

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    osDelay(offTime);
  }
}

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

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    Error_Handler();
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    HAL_Delay(200);
  }
}
