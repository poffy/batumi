

#include <stm32f10x_conf.h>

#include "stmlib/system/system_clock.h"
#include "drivers/system.h"
#include "drivers/leds.h"
#include "drivers/switches.h"
#include "drivers/dac.h"

using namespace batumi;
using namespace stmlib;

System sys;
Leds leds;
Switches switches;
Dac dac;

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
  void assert_failed(uint8_t* file, uint32_t line) { while (1); }
}

extern "C" {

  static __IO uint32_t TimingDelay;

  void SysTick_Handler() {
    if (TimingDelay != 0x00)
      TimingDelay--;
  }
}

void Delay(uint32_t nTime);

void Init() {
  sys.Init(F_CPU / 96000 - 1, true);
  system_clock.Init();
  leds.Init();
  switches.Init();
  dac.Init();

  sys.StartTimers();
}

void Delay(uint32_t nTime) {
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

int main(void) {

  Init();

  int16_t phase = 0;

  while (1) {
    Delay(1);
    for (int i=0; i<8; i++)
      dac.set(i, phase + i * INT16_MAX / 4);
    dac.Update();
    phase += 100;
  }
}
