

#include <stm32f10x_conf.h>

#include "stmlib/system/system_clock.h"
#include "drivers/system.h"
#include "drivers/leds.h"
#include "drivers/switches.h"

using namespace batumi;
using namespace stmlib;

System sys;
Leds leds;
Switches switches;

extern "C" {
  void HardFault_Handler(void) { while (1); }
  void MemManage_Handler(void) { while (1); }
  void BusFault_Handler(void) { while (1); }
  void UsageFault_Handler(void) { while (1); }
  void NMI_Handler(void) { }
  void SVC_Handler(void) { }
  void DebugMon_Handler(void) { }
  void PendSV_Handler(void) { }
  void __cxa_pure_virtual() { while (1); }
}

extern "C" {

static __IO uint32_t TimingDelay;

void SysTick_Handler() {
  if (TimingDelay != 0x00)
    TimingDelay--;
}

void TIM1_UP_IRQHandler(void) {
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) == RESET) {
    return;
  }

  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

void Delay(uint32_t nTime);

void Init() {
  sys.Init(F_CPU / 96000 - 1, true);
  system_clock.Init();
  sys.StartTimers();
  leds.Init();
  switches.Init();
}

void Delay(uint32_t nTime) {
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

int main(void) {

  Init();

  leds.set(1, 0);
  leds.set(2, 0);
  leds.set(3, 0);

  while (1) {
    static int ledval = 0;
    leds.set(0, ledval);
    ledval = 1 - ledval;
    leds.Write();
    Delay(250); // wait 250ms
  }
}
}

