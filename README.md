# VHAL

VHAL берет на себя конфигурацию переферии, все, что нужно, это объявить в BSP нужные GPIO и переферию которая будет использщоваться на уровне приложения.

Следует так же понимать, что VHAL не позволяет приложению менять распиновку (альтернативные пины) они зафиксированы в BSP.

BSP требует установить вручную на уровне LL (регистров):
- Система
  - Конфигурацию системы (актуально для старших версий, такие вещи как тактирование домена питания)
  - Конфигурацию часов ядра
  - Конфигурацию SysTick и ддобавление в прерывания обработчиков  для RTOS и `RTOS::HandleSysTickInterrupt()` и `System::TickHandler()`

- Переферия (Такие как I2C, UART, TIMER... кроме GPIO)
  - Инициализацию `EnableClock` для каждой переферии
  - Инициализацию GPIO в Alternate Mode
  - Инициализацию `NVIC_SetPriority` и `NVIC_EnableIRQ` если используются прерывания
  
# Создание проекта в CubeIDE и CubeMX
  
В качистве примера взят `STM32F412RETx`.

## Начальная настройка
- Создайте новый проект в CubeMX и настройте стандартные вещи, такие как тактирование и вклдючение SW отладки (SYS ->Debug -> Serial Wire)
- В качестве Timebase Sourse (SYS -> Timebase Sourse) используйте SysTick. Можно так же использовать таймер по своему желанию, но убедитесь, что он инициализируется первым поскольку некоторые адаптеры VHAL используют `System::TickHandler()` для инициализации (ADC может использовать задержку чтобы иметь время правильно откалиброваться)
- Включите FreeRTOS во вкладке Mindware, версия CMSYS не имеет значение, нам нужен только файл FreeRtos и чтобы проект сгенерировался с учетом того, что будет использоваться RTOS. Этот шаг так же можно пропустить, но множество драйверов и утилит используют RTOSAdapter, будьте осторожны.

## Настройка переферии
- Активируйте нужную вам переферию и ее прерывания (в том случае, если собираетесь использовать прерывания). Больше ничего не нужно, остальнон можно настроить непосредственно из VHAL.

## Генерацция проекта
- Перейдите во вкладку Project Manager -> и выберете Toolchain / IDE -> STM32CubeIDE
- Во вкладке Project Manager -> Advenced Settings выберете каждую переферию и смените HAL на LL (поскольку VHAL использует LL)
- Нажмите кнопку Generate Code (если CubeMX спросит, уверены ли вы, что хотите использовать SysTick то нажмите - да)

# Create VHAL Project
- Duplicate the newly generated project, it will be used as a code donor
- Open the original project

## Config IDE
- Donwload or clone [VHAL](https://github.com/VeyDlin/VHAL) and сopy `VHAL` to project root
- Copy `Application` and `BSP` to project root
- `Right mouse button` -> `Refresh` (or F5)
- `Right mouse button` -> `Convert to C++`
- `Right mouse button` -> `Properties` 
  - `C/C++ Build`
    - (advice) tab `Behavior` -> `Enable Parallel Build` and `Use unlimited jobs`
    - (advice) `Settings` -> `MCU GCC Compiler` -> `Geniral` -> `Standart` -> GNU18 (Update the cube if you don't have GNU++20)
    - `Settings` -> `MCU G++ Compiler` -> `Geniral` -> `Standart` -> GNU++20 (Update the cube if you don't have GNU++20)
  - `C/C++ Geniral` -> `Path and Symbols`
    - `Includes` -> ... (`Add to all configurations` and `Add to all languages`)
      - `Add` -> `Directory`: `Application`
      - `Add` -> `Directory`: `BSP`
      - `Add` -> `Directory`: `VHAL/Common`
      - `Add` -> `Directory`: `VHAL/Periphery`
    - `Source Location`
      - `Add Folder...` -> `Application`
      - `Add Folder...` -> `BSP`
      - `Add Folder...` -> `VHAL`

## Config VHAL Project
- Delete all files from the `Core` folder except `FreeRTOSConfig.h`, `main.c`, `freertos.c`, `system_stm32XXX.c`
- Rename `main.c` to `main.cpp`. It should look something like this:
  - `Core`
    - `Inc`
      - FreeRTOSConfig.h
    - `Src`
      - freertos.c
      - main.cpp
      - system_stm32f4xx.c (The name differs depending on the version of the STM32)
      
 - Delete everything from `main.cpp` and paste this code
 
   ```c++
    #include <Application.h>

    int main(void) {
      BSP::Init();
      Application::Init();
    }
   ```
    
