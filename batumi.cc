
#include <stm32f10x_conf.h>

#include "stmlib/system/system_clock.h"
#include "drivers/system.h"
#include "drivers/dac.h"
#include "drivers/adc.h"

#include "ui.h"
#include "lfo.h"

using namespace batumi;
using namespace stmlib;

const uint8_t kNumChannels = 4;

System sys;
Dac dac;
Adc adc;
Ui ui;
Lfo lfo[kNumChannels];

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
  sys.Init(F_CPU / SAMPLE_RATE - 1, true);
  system_clock.Init();
  adc.Init();
  ui.Init(&adc, lfo); // must be after adc
  dac.Init();
  for (int i=0; i<kNumChannels; i++)
    lfo[i].Init();

  sys.StartTimers();
}

int main(void) {
  Init();
  while(1) {
    ui.DoEvents();
  }
}

int16_t AdcValuesToPitch(uint16_t pot, int16_t cv) {
  return -4656 +
    ((pot - 32768) * 10205 >> 15) +
    (cv * 5 * 12 * 128 >> 15);
}

void Process() {

  if(ui.mode() == UI_MODE_SPLASH)
    return;

  switch (ui.feat_mode()) {
  case FEAT_MODE_FREE:
  {
    for (int i=0; i<kNumChannels; i++) {
      int16_t pitch = AdcValuesToPitch(ui.pot(i), adc.cv(i));
      lfo[i].set_pitch(pitch);
    }
  }
  break;
  case FEAT_MODE_QUAD:
  {
    int16_t pitch = AdcValuesToPitch(ui.pot(0), adc.cv(0));
    lfo[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo[i].set_pitch(pitch);
      int32_t level = ui.pot(i) + adc.cv(i);
      CONSTRAIN(level, 0, UINT16_MAX);
      lfo[i].set_level(level);
      lfo[i].set_phase((kNumChannels - i) * (UINT16_MAX >> 2));
    }
  }
  break;
  case FEAT_MODE_PHASE:
  {
    int16_t pitch = AdcValuesToPitch(ui.pot(0), adc.cv(0));
    lfo[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo[i].set_pitch(pitch);
      lfo[i].set_phase(ui.pot(i) + adc.cv(i));
    }
  }
  break;
  case FEAT_MODE_DIVIDE:
  {
    const int8_t kDivisions[6] = {2, 3, 4, 8, 16, 32};
    int16_t pitch = AdcValuesToPitch(ui.pot(0), adc.cv(0));
    lfo[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo[i].set_pitch(pitch);
      int32_t ctrl = ui.pot(i) + adc.cv(i);
      CONSTRAIN(ctrl, 0, UINT16_MAX);
      uint8_t div_index = (static_cast<uint32_t>(65535 - ctrl) * 6) >> 16;
      lfo[i].set_divider(kDivisions[div_index]);
    }
  }
  break;
  case FEAT_MODE_LAST: break;	// to please the compiler
  }

  // send to DAC
  for (int i=0; i<kNumChannels; i++) {
    lfo[i].Step();
    dac.set_sine(i, lfo[i].ComputeSampleSine());
    dac.set_asgn(i, lfo[i].ComputeSampleShape(ui.shape()));
  }
}

extern "C" {

  void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == RESET) {
      return;
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    adc.Scan();
    Process();
    dac.Write();
  }
  
}
