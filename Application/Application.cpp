#include <Application.h>
#include <AppThreads/MainTask.h>



MainTask Application::main;




void Application::Init() {
	InitSystemHandle();

	RTOS::CreateThread(main);

	RTOS::Start();
}





void Application::InitSystemHandle() {

	// For use System::DelayMs() in RTOS
	System::rtosDelayMsHandle = [](uint32 delay) {
		if(RTOS::IsSchedulerRun()) {
			RTOS::Sleep(std::chrono::milliseconds(delay));
			return true;
		}
		return false;
	};


	// For use std::cout and printf and System::log
	System::writeHandle = [](char* string, size_t size) {
		
	};

	System::readHandle = [](char* string, size_t size) {
		
	};


	// For log System::CriticalError() and SystemAbort()
	System::criticalErrorHandle = [](char* string, uint32 line) {
		// Save to FLASH or/and log
		// Error in (string) file on (line) line code
	};
}


