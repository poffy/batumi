// Copyright 2015 Matthias Puech.
//
// Author: Matthias Puech (matthias.puech@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Batumi, main file

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
