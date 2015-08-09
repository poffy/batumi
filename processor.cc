#include "drivers/adc.h"
#include "drivers/dac.h"

#include "processor.h"
#include "ui.h"

namespace batumi {

const uint8_t kDivisions[6] = {2, 3, 4, 8, 16, 32};
const int16_t kUnsyncThreshold = INT16_MAX / 16;

void Processor::Init(Ui *ui, Adc *adc, Dac *dac) {
  ui_ = ui;
  adc_ = adc;
  dac_ = dac;
  previous_feat_mode_ = FEAT_MODE_LAST;
  // no need to Init the LFOs, it'll be done in Process on first run
  for (uint8_t i=0; i<kNumChannels; i++) {
    reset_trigger_armed_[i]= false;
    last_reset_[i] = 0;
  }
}

inline int16_t AdcValuesToPitch(uint16_t pot, int16_t cv) {
  return -4656 +
    ((pot - 32768) * 10205 >> 15) +
    (cv * 5 * 12 * 128 >> 15);
}

inline uint8_t AdcValuesToDivider(uint16_t pot, int16_t cv) {
  int32_t ctrl = pot + cv;
  CONSTRAIN(ctrl, 0, UINT16_MAX);
  uint8_t div_index = (static_cast<uint32_t>(65535 - ctrl) * 6) >> 16;
  return kDivisions[div_index];
}

void Processor::Process() {

  // do not run during the splash animation
  if (ui_->mode() == UI_MODE_SPLASH)
    return;

  // reset the LFOs if mode changed
  if (ui_->feat_mode() != previous_feat_mode_) {
    for (int i=0; i<kNumChannels; i++)
      lfo_[i].Init();
    previous_feat_mode_ = ui_->feat_mode();
  }

  // detect triggers on the reset input
  for (uint8_t i=0; i<kNumChannels; i++) {
    int16_t reset = adc_->reset(i);
    if (reset < 10000)
      reset_trigger_armed_[i] = true;
    reset_triggered_[i] = reset > 20000 &&
      reset_trigger_armed_[i] &&
      last_reset_[i] > 100;
  }

  switch (ui_->feat_mode()) {
  case FEAT_MODE_FREE:
  {
    for (uint8_t i=0; i<kNumChannels; i++) {
      int16_t pitch = AdcValuesToPitch(ui_->pot(i), adc_->cv(i));

      // on reset rising edge:
      if (reset_triggered_[i]) {
	if (ui_->sync_mode()) {
	  lfo_[i].set_period(last_reset_[i]);
	  synced_[i] = true;
	} else {			// reset mode
	  lfo_[i].Reset();
	}
	reset_trigger_armed_[i] = false;
	last_reset_[i] = 0;
      } else {
	last_reset_[i]++;
      }

      if (!synced_[i] ||
	  (abs(pitch - last_pitch_[i]) > kUnsyncThreshold)) {
	lfo_[i].set_pitch(pitch);
	last_pitch_[i] = pitch;
	synced_[i] = false;
      }

    }
  }
  break;
  case FEAT_MODE_QUAD:
  {
    int16_t pitch = AdcValuesToPitch(ui_->pot(0), adc_->cv(0));
    lfo_[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].set_pitch(pitch);
      lfo_[i].set_phase((kNumChannels - i) * (UINT16_MAX >> 2));
    }
  }
  break;
  case FEAT_MODE_PHASE:
  {
    int16_t pitch = AdcValuesToPitch(ui_->pot(0), adc_->cv(0));
    lfo_[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].set_pitch(pitch);
      lfo_[i].set_phase(ui_->pot(i) + adc_->cv(i));
    }
  }
  break;
  case FEAT_MODE_DIVIDE:
  {
    int16_t pitch = AdcValuesToPitch(ui_->pot(0), adc_->cv(0));
    lfo_[0].set_pitch(pitch);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].set_pitch(pitch);
      lfo_[i].set_divider(AdcValuesToDivider(ui_->pot(i), adc_->cv(i)));

    }
  }
  break;
  case FEAT_MODE_LAST: break;	// to please the compiler
  }

  // send to DAC
  for (int i=0; i<kNumChannels; i++) {
    lfo_[i].Step();
    LfoShape shape = static_cast<LfoShape>(ui_->shape());
    dac_->set_sine(i, lfo_[i].ComputeSampleSine());
    dac_->set_asgn(i, lfo_[i].ComputeSampleShape(shape));
  }
}
}
