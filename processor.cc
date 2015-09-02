#include "drivers/adc.h"
#include "drivers/dac.h"

#include "processor.h"
#include "ui.h"
#include "resources.h"
#include "stmlib/utils/dsp.h"

namespace batumi {

using namespace stmlib;

const int16_t kUnsyncPotThreshold = INT16_MAX / 20;

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

  pot = Interpolate88(lut_scale_freq, pot) - 32768;
  cv = cv * 5 * kOctave >> 15;
  return pot + cv;
}

inline uint8_t AdcValuesToDivider(uint16_t pot, int16_t cv) {
  int32_t ctrl = pot + cv;
  CONSTRAIN(ctrl, 0, UINT16_MAX);
  return Interpolate88(lut_scale_divide, ctrl);
}

inline uint16_t AdcValuesToPhase(uint16_t pot, int16_t cv) {
  int32_t ctrl = pot + cv;
  // no need to clip the result, wrapping around is ok
  return Interpolate88(lut_scale_phase, ctrl);
}

void Processor::SetFrequency(int8_t lfo_no) {
  int16_t reset = adc_->reset(lfo_no);

  // detect triggers on the reset input
  if (reset < 10000)
    reset_trigger_armed_[lfo_no] = true;
  reset_triggered_[lfo_no] = reset > 20000 &&
    reset_trigger_armed_[lfo_no] &&
    last_reset_[lfo_no] > 100;

  // sync or reset
  if (reset_triggered_[lfo_no]) {
    if (ui_->sync_mode()) {
      lfo_[lfo_no].set_period(last_reset_[lfo_no]);
      synced_[lfo_no] = true;
    } else {			// reset mode
      lfo_[lfo_no].Reset();
    }
    reset_trigger_armed_[lfo_no] = false;
    last_reset_[lfo_no] = 0;
  } else {
    last_reset_[lfo_no]++;
  }

  int16_t pitch = AdcValuesToPitch(ui_->pot(lfo_no), adc_->cv(lfo_no));

  // set pitch
  if (!synced_[lfo_no] ||
      (abs(pitch - last_pitch_[lfo_no]) > kUnsyncPotThreshold)) {
    lfo_[lfo_no].set_pitch(pitch);
    last_pitch_[lfo_no] = pitch;
    synced_[lfo_no] = false;
  }
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

  switch (ui_->feat_mode()) {
  case FEAT_MODE_FREE:
  {
    for (uint8_t i=0; i<kNumChannels; i++) {
      SetFrequency(i);
    }
  }
  break;

  case FEAT_MODE_QUAD:
  {
    SetFrequency(0);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].link_to(&lfo_[0]);
      lfo_[i].set_initial_phase((kNumChannels - i) * (UINT16_MAX >> 2));
    }
  }
  break;

  case FEAT_MODE_PHASE:
  {
    SetFrequency(0);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].link_to(&lfo_[0]);
      lfo_[i].set_initial_phase(AdcValuesToPhase(ui_->pot(i), adc_->cv(i)));
    }
  }
  break;

  case FEAT_MODE_DIVIDE:
  {
    SetFrequency(0);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].link_to(&lfo_[0]);
      lfo_[i].set_divider(AdcValuesToDivider(ui_->pot(i), adc_->cv(i)));
      // we also need to reset the divider count:
      if (reset_triggered_[0]) {
	lfo_[i].Reset();
      }
    }
  }
  break;

  case FEAT_MODE_LAST: break;	// to please the compiler
  }

  // send to DAC and step
  for (int i=0; i<kNumChannels; i++) {
    lfo_[i].Step();
    LfoShape shape = static_cast<LfoShape>(ui_->shape());
    dac_->set_sine(i, lfo_[i].ComputeSampleSine());
    dac_->set_asgn(i, lfo_[i].ComputeSampleShape(shape));
  }
}
}
