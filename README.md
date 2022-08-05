# Start
- Duplicate the newly generated project, it will be used as a code donor
- Open the original project

# Config IDE
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

# Config VHAL
- Delete all files from the `Core` folder except `FreeRTOSConfig.h`, `main.c`, `freertos.c`, `system_stm32XXX.c`
- Rename `main.c` to `main.cpp`

  It should look something like this
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
    
