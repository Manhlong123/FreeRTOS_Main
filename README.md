# FreeRTOS LED Blink Demo for STM32F103C8T6 (Blue Pill)

## Mô tả
Dự án minh họa cách sử dụng **FreeRTOS (CMSIS-RTOS wrapper)** với **2 Task** và **Queue** trên STM32F103C8T6.
- LED: `PA0`
- System Clock: `72 MHz` (HSE 8 MHz x9)
- FreeRTOS: cấu hình trong `User/FreeRTOSConfig.h`

## Cấu trúc
```
FreeRTOS_LED_Demo/
├─ MDK-ARM/                # Keil project files (.uvprojx, .uvoptx)
└─ User/
   ├─ main.c               # source (đã cung cấp)
   ├─ main.h
   ├─ freertos.c
   ├─ freertos.h
   └─ FreeRTOSConfig.h
```

## Hướng dẫn build (Keil µVision5)
1. Mở `MDK-ARM/FreeRTOS_LED_Demo.uvprojx` trong Keil µVision5.
2. Thêm CMSIS/Device/Startup/System files nếu Keil yêu cầu (nếu bạn đã cài STM32F1xx Device Pack thì thường không cần).
3. Build và nạp vào board (Blue Pill).
4. Quan sát LED ở chân `PA0` nhấp nháy; Task03 sẽ gửi các cặp frequency/duty mỗi 3s.

## Chú ý
- Project giả định bạn đã có **STM32F1xx HAL** và **CMSIS/RTOS** trong môi trường Keil (Device Pack).
- Nếu Keil báo thiếu file startup hoặc system_stm32f1xx.c, hãy thêm chúng từ STM32Cube HAL package hoặc Device Pack.

## Liên hệ
Nếu bạn muốn mình bổ sung startup file, linker script (.sct) hoặc cấu hình debug cụ thể, nói mình biết.