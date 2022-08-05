#include "SystemIrq.h"
#include <System/System.h>
#include <BSP.h>
#include <Application.h>
#include <Adapter/OSAdapter/RTOS.h>



void NMI_Handler() {
	
}



void HardFault_Handler() {
	System::CriticalError("HardFault");
}



void MemManage_Handler() {
	System::CriticalError("MemManage");
}



void BusFault_Handler() {
	System::CriticalError("BusFault");
}



void UsageFault_Handler() {
	System::CriticalError("UsageFault");
}



void DebugMon_Handler() {

}



void SysTick_Handler() {
	System::TickHandler();
	OSAdapter::RTOS::HandleSysTickInterrupt();
}



