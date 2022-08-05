# VHAL
VHAL - С++20 HAL library for STM32.

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
- Перейдите во вкладку `Project Manager` -> и выберете `Toolchain / IDE` -> `STM32CubeIDE`
- Во вкладке `Project Manager` -> `Advenced Settings` выберете каждую переферию и смените `HAL` на `LL` (поскольку VHAL использует LL)
- Нажмите кнопку `Generate Code` (если CubeMX спросит, уверены ли вы, что хотите использовать SysTick то нажмите - да)

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

## Project preparation
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

## Config VHAL Project base
Откройте проект-донор чтобы копировать от туда конфигурации. В дальнейшем, если нужно будет добавить новую переферию или изменить существующую, используйте CubeMX чтобы перегенерировать проект-донор и скопировать конфигурации по инструкциям ниже.

- Откройте файл (донор) `Core\Inc\main.h` и скопируйте все include и PRIORITYGROUP (если они есть) в `BSP/Periphery.h` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/Periphery.h))
- Адаптеры
  - Откройте `BSP/PortAdapters.h` и добавьте include адаптеров используемой переферии для вашей серии STM32 в формате `#include <Adapter/Port/F_X_/_PERIPHERY_AdapterF_X_.h>` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/PortAdapters.h))
    - Обратите внимание, что у вас будет ошибка компиляции если вы добавите адаптеры той переферии, которую не выбрали в CubeMX поскольку CubeMX генерирует файлы LL библиотек только для той переферии, которая активирована.
- Прерывания (этот шаг необязательный и только для ознакомления поскольку в шаблоне уже написан нужный код)
  - Откройте файл (донор) `Core\Inc\stm32f4xx_it.h` (название может отличатся от серии STM32) и скопируйте functions prototypes в `BSP/IRQ/SystemIrq.h` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/IRQ/SystemIrq.h))
  - Откройте файл (донор) `Core\Src\stm32f4xx_it.c` (название может отличатся от серии STM32) и скопируйте Exception Handlers в `BSP/IRQ/SystemIrq.cpp` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/IRQ/SystemIrq.cpp))
  Добавьте в `SysTick_Handler` обработчики для `RTOS` и `System` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/IRQ/SystemIrq.cpp#L45)):
  
    ```c++
    System::TickHandler();
    OSAdapter::RTOS::HandleSysTickInterrupt();
    ```
  В случае исли вы используете вместо SysTick таймер, следует добавить код туда.
- Система
  -  Откройте файл (донор) `Core\Src\main.c`
    - В функции `int main(void)` до вызова `SystemClock_Config()` в некоторых версиях STM32 существует MCU Configuration, код в `BSP/BSP.cpp` -> `BSP::InitSystem()` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L11)). Если у вас нет этого кода, то пропустите этот шаг.
    - В функции `void SystemClock_Config(void)` скопируйте все содержимое в `BSP/BSP.cpp` -> `BSP::InitClock()` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L30))
      - CubeMX использует [LL_Init1msTick](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L56) для конфигурации частоты прерывания `SysTick_Handler`, но мы уже используем [BSP::InitSystemTick](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L65), так что строку с `LL_Init1msTick` можно закоментировать, либо вставить ее в `BSP::InitSystemTick` по вашему усмотрению.

На этом этапе проект уже должен успешно компилироваться и работать.

## Config VHAL Project periphery
Для примера мы будем использовать UART, если вы не включили его в проект то это отличная возможность потренироваться.

В случае, если вы решили добавить совершенно новую переферю:
- Включите UART с прерываниями в проекте-доноре и после запустите генератор CubeMX. (не забудьте сменить `HAL` на `LL` во вкладке `Project Manager` -> `Advenced Settings`)
- Скопируйте новые библиотеки которые лежат в папке `Drivers/STM32F4xx_HAL_Driver` (название может отличатся от серии STM32)
  - (advice) Легче всего просто скопировать всю папку `Drivers` из донор-проекта в оригинальный проект с заменой.
- Добавьте новый include вашей переферии в `BSP/PortAdapters.h`

### Добавление GPIO:

Простые GPIO добавлять крайне просто.
- Откройте `BSP/BSP.h` и создайте `public` переменную `static AGPIO` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.h#L10))

  ```c++
  class BSP {
  ...
  public:
    ...
    static AGPIO ledPin;
  ```
- Откройте `BSP/BSP.cpp` и объявите вашу переменную ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L6))

  ```c++
  AGPIO BSP::ledPin	= { GPIOC, 6 };
  ```
  В данном случае это GPIOC6. Если вы хотите чтобы при установке пина его сигнал был низким (инверсная логика) то можете объявить его так:
  
  ```c++
  AGPIO BSP::ledPin	= { GPIOC, 6, true };
  ```
  3 необязательный параметр задает, используется ли инверсная логика, по дефолту это `false`
  
### Добавление другой переферии:

Для примера используется UART. 
- Откройте `BSP/BSP.h` и создайте `public` переменную `static AUART` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.h#L8))

  ```c++
  class BSP {
  ...
  public:
    ...
    static AUART consoleSerial;
  ```
- Откройте `BSP/BSP.cpp` и объявите вашу переменную ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L4))

  ```c++
  AUART BSP::consoleSerial = { USART1 };
  ```
  В данном случае используется `USART1`, но у вас может быть другой.
- Откройте файл (донор) `Core\Src\main.c` и найдите вашу функцию инициализации UART, например `void MX_USART1_UART_Init()`
  Вам нужно только функция включения часов UART и GPIO на которых он будет работать.
  - Найдите в `void MX_USART1_UART_Init()` peripheral clock enable, в данном случае это:
  
    ```c++
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    ```
  Обратите внимаение, что peripheral clock enable для GPIO вам не нужен, он будет включен автоматически VHAL.
  - Найдите в `void MX_USART1_UART_Init()` interrupt Init, в данном случае это:
    (этот шаг можно пропустить если вы не используете прерывания) 
    
    ```c++
    /* USART1 interrupt Init */
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART1_IRQn);
    ```
  - Найдите UART GPIO Configuration и запомните GPIO и их Alternate Mode
    ```c++
    /** USART1 GPIO Configuration
        PA7   ------> USART1_TX
        PA10   ------> USART1_RX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    ```
  В данном случае это `PA7` и `PA10`, оба используют 7 Alternate Mode (`GPIO_InitStruct.Alternate = LL_GPIO_AF_7;`)
- Откройте `BSP/BSP.cpp` и добавьте в `BSP::InitAdapterPeripheryEvents()` событие `beforePeripheryInit` для вашей перменной ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/BSP.cpp#L76)):

  ```c++
	consoleSerial.beforePeripheryInit = []() {
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

		AGPIO::AlternateInit<AGPIO>({ GPIOA, 9,  7, AGPIO::Pull::Up, AGPIO::Speed::VeryHigh });
		AGPIO::AlternateInit<AGPIO>({ GPIOA, 10, 7, AGPIO::Pull::Up, AGPIO::Speed::VeryHigh });

		NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
		NVIC_EnableIRQ(USART1_IRQn);

		return Status::ok;
	};
  ```
  `EnableClock`, `SetPriority`, `EnableIRQ` копируются из проекта-донора, для GPIO есть удобный интерфейс `AGPIO::AlternateInit`
- И наконец-то, если вы используете прерывания, добавьте обработчик прерываний в BSP
  - Откройте файл (донор) `Core\Inc\stm32f4xx_it.h` (название может отличатся от серии STM32) и скопируйте functions prototypes в `BSP/IRQ/DeviceIrq.h` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/IRQ/DeviceIrq.h#L7))
  - Откройте файл (донор) `Core\Src\stm32f4xx_it.c` (название может отличатся от серии STM32) и скопируйте Exception Handlers в `BSP/IRQ/DeviceIrq.cpp` ([пример](https://github.com/VeyDlin/VHAL_Template/blob/main/BSP/IRQ/DeviceIrq.cpp#L9))
  
  После добавьте обработчик VHAL для вашего UART:
  
  ```c++
  BSP::consoleSerial.IrqHandler();
  ```
  
# Готово
Вы создали проект и сконфигурировали BSP, теперь вы можете использовать вашу переферию, о том, как ее конфигурировать на верхнем уровне можно почитать в документации к [VHAL](https://github.com/VeyDlin/VHAL)
