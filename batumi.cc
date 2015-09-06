
#include <stm32f10x_conf.h>

#include "stmlib/system/system_clock.h"
#include "drivers/system.h"
#include "drivers/dac.h"
#include "drivers/adc.h"
#include "stmlib/utils/random.h"
#include "stmlib/system/uid.h"
#include "ui.h"
#include "processor.h"

using namespace batumi;
using namespace stmlib;

System sys;
Dac dac;
Adc adc;
Ui ui;
Processor processor;

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

void Init() {
  sys.Init(F_CPU / SAMPLE_RATE - 1, true);
  system_clock.Init();
  adc.Init();
  ui.Init(&adc); // must be after adc
  dac.Init();
  processor.Init(&ui, &adc, &dac);
  Random::Seed(GetUniqueId(1));

  sys.StartTimers();
}

int main(void) {
  Init();
  while(1) {
    ui.DoEvents();
    __WFI();
  }
}

extern "C" {

  // slow timer for the UI
  void SysTick_Handler() {
    system_clock.Tick();  // Tick global ms counter.
    ui.Poll();
  }

  // fast timer for processing
  void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == RESET) {
      return;
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    adc.Scan();
    processor.Process();
    dac.Write();
  }
  
}
