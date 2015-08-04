

#include <stm32f10x_conf.h>

#include "stmlib/system/system_clock.h"
#include "drivers/system.h"
#include "drivers/switches.h"
#include "drivers/dac.h"
#include "drivers/adc.h"

#include "ui.h"

using namespace batumi;
using namespace stmlib;

uint8_t kNumChannels = 4;

System sys;
Dac dac;
Adc adc;
Ui ui;
// Lfo lfo[kNumChannels];

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

  void SysTick_Handler() {
    system_clock.Tick();  // Tick global ms counter.
    ui.Poll();
  }
}

void Init() {
  // System clock at 25 Hz
  sys.Init(F_CPU / 2880000 - 1, true);
  system_clock.Init();
  ui.Init(&adc);
  adc.Init();
  dac.Init();

  sys.StartTimers();
}

int main(void) {

  Init();

  while (1) {
    ui.DoEvents();

    adc.Scan();
    dac.set(0, adc.value(ADC_POT1));
    dac.set(1, adc.value(ADC_POT2));
    dac.set(2, adc.value(ADC_POT3));
    dac.set(3, adc.value(ADC_POT4));
    dac.Update();
  }
}
