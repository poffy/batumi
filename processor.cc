#include "drivers/adc.h"
#include "drivers/dac.h"

#include "processor.h"
#include "ui.h"
#include "resources.h"
#include "stmlib/utils/dsp.h"

namespace batumi {

using namespace stmlib;

const int16_t kUnsyncPotThreshold = INT16_MAX / 20;
const int16_t kResetThresholdLow = 10000;
const int16_t kResetThresholdHigh = 20000;
const int16_t kHoldThreshold = -10000;

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

inline int16_t AdcValuesToPitch(uint16_t coarse, int16_t fine, int16_t cv) {
  coarse = Interpolate88(lut_scale_freq, coarse) - 32768;
  fine = (1 * kOctave * static_cast<int32_t>(fine)) >> 16;
  cv = cv * 5 * kOctave >> 15;
  return coarse + fine + cv;
}

inline uint8_t AdcValuesToDivider(uint16_t pot, int16_t fine, int16_t cv) {
  int32_t ctrl = pot + cv;
  CONSTRAIN(ctrl, 0, UINT16_MAX);
  fine = (5 * static_cast<int32_t>(fine + INT16_MAX / 5)) >> 16;
  int8_t div = Interpolate88(lut_scale_divide, ctrl);
  div -= fine;
  CONSTRAIN(div, 1, 64);
  return div;
}

inline uint16_t AdcValuesToPhase(uint16_t pot, int16_t fine, int16_t cv) {
  int32_t ctrl = pot + cv + fine / 8;
  // no need to clip the result, wrapping around is ok
  return Interpolate88(lut_scale_phase, ctrl);
}

void Processor::SetFrequency(int8_t lfo_no) {
  int16_t reset = adc_->reset(lfo_no);

  // detect triggers on the reset input
  if (reset < kResetThresholdLow)
    reset_trigger_armed_[lfo_no] = true;

  if (reset > kResetThresholdHigh &&
      reset_trigger_armed_[lfo_no]) {
    reset_triggered_[lfo_no] = true;
    int32_t dist_to_trig = kResetThresholdHigh - previous_reset_[lfo_no];
    int32_t dist_to_next = reset - previous_reset_[lfo_no];
    reset_subsample_[lfo_no] = dist_to_trig * 32L / dist_to_next;
  } else {
    reset_triggered_[lfo_no] = false;
  }

  previous_reset_[lfo_no] = reset;

  // // hold if negative reset
  // if (reset < kHoldThreshold) {
  //   lfo_[lfo_no].set_pitch(INT16_MIN);
  //   return;
  // }

  // sync or reset
  if (reset_triggered_[lfo_no]) {
    if (ui_->sync_mode()) {
      lfo_[lfo_no].set_period(last_reset_[lfo_no]);
      synced_[lfo_no] = true;
    }
    lfo_[lfo_no].Reset(reset_subsample_[lfo_no]);
    reset_trigger_armed_[lfo_no] = false;
    last_reset_[lfo_no] = 0;
  } else {
    last_reset_[lfo_no]++;
  }

  int16_t pitch = AdcValuesToPitch(ui_->coarse(lfo_no),
				   ui_->fine(lfo_no),
				   adc_->cv(lfo_no));

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
      lfo_[i].set_initial_phase(AdcValuesToPhase(ui_->coarse(i),
						 ui_->fine(i),
						 adc_->cv(i)));
    }
  }
  break;

  case FEAT_MODE_DIVIDE:
  {
    SetFrequency(0);
    for (int i=1; i<kNumChannels; i++) {
      lfo_[i].link_to(&lfo_[0]);
      lfo_[i].set_divider(AdcValuesToDivider(ui_->coarse(i),
					     ui_->fine(i),
					     adc_->cv(i)));
      // we also need to reset the divider count:
      if (reset_triggered_[0]) {
	lfo_[i].Reset(reset_subsample_[0]);
      }
    }
  }
  break;

  case FEAT_MODE_LAST: break;	// to please the compiler
  }

  // send to DAC and step
  for (int i=0; i<kNumChannels; i++) {
    lfo_[i].Step();
    LfoShape shape = static_cast<LfoShape>(ui_->shape()+1);
    dac_->set_sine(i, lfo_[i].ComputeSampleShape(SHAPE_SINE));
    dac_->set_asgn(i, lfo_[i].ComputeSampleShape(shape));
  }
}
}
