#include "BSP.h"





void BSP::InitSystem() {

}





void BSP::InitClock() {

}





void BSP::InitSystemTick(uint32 ms, uint32 tickPriority) {
	assert(SysTick_Config(System::GetCoreClock() / (1000U / ((1000U * ms) / 1000U))) == 0);
	assert(tickPriority < (1UL << __NVIC_PRIO_BITS));
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), tickPriority, 0));
}





void BSP::InitAdapterPeripheryEvents() {

}





void BSP::InitIO() {

}














