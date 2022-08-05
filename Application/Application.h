#pragma once
#include <BSP.h>

#include <Adapter/OSAdapter/RTOS.h>
#include <Adapter/OSAdapter/Thread.h>
#include <Adapter/OSAdapter/Mutex.h>
#include <Adapter/OSAdapter/MailBox.h>



using namespace OSAdapter;





class Application {
private:
	static constexpr bool useWatchDog = false;


public:
	static class MainTask main;
	static class McuWatchDogTask mcuWatchDog;


public:
	static void Init();


private:
	static void InitSystemHandle();

};

