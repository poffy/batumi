// Copyright 2013 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
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
// LFO.

#include "lfo.h"

#include <cstdio>

#include "stmlib/utils/dsp.h"
#include "stmlib/utils/random.h"

#include "resources.h"

namespace batumi {

using namespace stmlib;

void Lfo::Init() {
  phase_ = 0;
  alignment_phase_ = 0;
  divided_phase_ = 0;
  multiplied_phase_ = 0;
  initial_phase_ = 0;
  phase_increment_ = UINT32_MAX >> 8;
  divider_ = 1;
  multiplier_ = 1;
  divider_counter_ = 0;
  cycle_counter_ = 0;
  level_ = UINT16_MAX;
  current_value_ = UINT16_MAX / 2;
  next_value_ = 0;
  logistic_seed_ = 368 + (Random::GetWord() >> 27);
}

void Lfo::Step() {
  phase_ += phase_increment_;
  if (phase_ < phase_increment_) {
    divider_counter_ = (divider_counter_ + 1) % divider_;
    cycle_counter_++;
  }
  divided_phase_ = phase_ / divider_ +
    (UINT32_MAX - UINT32_MAX / 750) / divider_ * divider_counter_;
  // that subtraction is a bit unexplainable, but it avoids an
  // overflow which advances the divided phases very slightly...
  multiplied_phase_ = divided_phase_ * multiplier_;

  // compute the next random value
  if (phase() < phase_increment_ / divider_ * multiplier_) {
    ComputeNextRandom();
  }
}

void Lfo::ComputeNextRandom() {
    current_value_ = next_value_;
    switch (random_type_) {
    case RANDOM_WHITE:
      next_value_ = Random::GetSample(); break;
    case RANDOM_LOGISTIC:
      current_value_ = next_value_;
      uint16_t x = current_value_ + 32768;
      uint16_t z = (x * (UINT16_MAX - x)) >> 16;
      next_value_ = (z * logistic_seed_ / 100) - 32768;
      if (cycle_counter_ & (1 << 7)) next_value_ += Random::GetSample() >> 12;
      break;
    }
}

void Lfo::Reset(uint8_t subsample) {
  /* save the current osc. value and compute the future value at the
   * end of the reset step */
  uint32_t end_phase = WAV_BL_STEP0_SIZE *
    phase_increment_ / divider_ * multiplier_;
  for (int i=0; i<kNumLfoShapes; i++) {
    LfoShape s = static_cast<LfoShape>(i);
    step_begin_[i] = ComputeSampleShape(s, phase());
    step_end_[i] = ComputeSampleShape(s, end_phase);
  }

  // reset phase etc.
  phase_ = 0;
  alignment_phase_ = 0;
  divider_counter_ = 0;
  cycle_counter_ = 0;
  ComputeNextRandom();
  // and start the reset step
  bl_step_counter_ = WAV_BL_STEP0_SIZE;
  reset_subsample_ = subsample;
}

uint32_t Lfo::ComputePhaseIncrement(int16_t pitch) {
  int16_t num_shifts = 0;
  while (pitch < 0) {
    pitch += kOctave;
    --num_shifts;
  }
  while (pitch >= kOctave) {
    pitch -= kOctave;
    ++num_shifts;
  }
  // Lookup phase increment
  uint32_t a = lut_increments[pitch >> 4];
  uint32_t b = lut_increments[(pitch >> 4) + 1];
  uint32_t phase_increment = a + ((b - a) * (pitch & 0xf) >> 4);
  return num_shifts >= 0
      ? phase_increment << num_shifts
      : phase_increment >> -num_shifts;
}

inline int16_t Lfo::ComputeSampleShape(LfoShape s, uint32_t phase) {
  switch (s) {
  case SHAPE_SINE:
    return ComputeSampleSine(phase);
  case SHAPE_TRIANGLE:
    return ComputeSampleTriangle(phase);
  case SHAPE_SAW:
    return ComputeSampleSaw(phase);
  case SHAPE_RAMP:
    return ComputeSampleRamp(phase);
  case SHAPE_TRAPEZOID:
    return ComputeSampleTrapezoid(phase);
  case SHAPE_SQUARE:
    return ComputeSampleSquare(phase);
  case SHAPE_RANDOM_STEP:
    random_type_ = RANDOM_WHITE;
    return ComputeSampleRandom(phase, false);
  case SHAPE_RANDOM_SMOOTH:
    random_type_ = RANDOM_WHITE;
    return ComputeSampleRandom(phase, true);
  case SHAPE_LOGISTIC_STEP:
    random_type_ = RANDOM_LOGISTIC;
    return ComputeSampleRandom(phase, false);
  case SHAPE_LOGISTIC_SMOOTH:
    random_type_ = RANDOM_LOGISTIC;
    return ComputeSampleRandom(phase, true);
  }
  return 0;			// never reached
}

int16_t Lfo::ComputeSampleShape(LfoShape s) {
  if (bl_step_counter_ == 0) {
    return ComputeSampleShape(s, phase());
  }

  int32_t end = step_begin_[s];
  int32_t begin = step_end_[s];

  int32_t step = waveform_table[WAV_BL_STEP0 + reset_subsample_][bl_step_counter_];
  step = (begin - end) * step / 30000 + end;
  CONSTRAIN(step, INT16_MIN, INT16_MAX);
  bl_step_counter_--;
  return step;
}

int16_t Lfo::ComputeSampleSine(uint32_t phase) {
  int16_t sine = Interpolate1022(wav_sine, phase);
  return -sine * level_ >> 16;
}

int16_t Lfo::ComputeSampleTriangle(uint32_t phase) {
  int16_t tri = phase < 1UL << 31
      ? -32768 + (phase >> 15)
      :  32767 - (phase >> 15);
  uint32_t pi = phase_increment_ / divider_ * multiplier_ >> 16;
  int16_t x = 0;
  if (pi > kPI100Hz) {
    x = Interpolate1022(wav_tri100, phase);
  } else if (pi > kPI10Hz) {
    uint16_t balance = (pi - kPI10Hz) * 65535L / (kPI100Hz - kPI10Hz);
    x = Crossfade1022(wav_tri10, wav_tri100, phase, balance);
  } else if (pi > kPI1Hz) {
    uint16_t balance = (pi - kPI1Hz) * 65535L / (kPI10Hz - kPI1Hz);
    int32_t a = tri;
    int32_t b = Interpolate1022(wav_tri10, phase);
    x = a + ((b - a) * static_cast<int32_t>(balance) >> 16);
  } else {
    x = tri;
  }
  return x * level_ >> 16;
}

int16_t Lfo::ComputeSampleSaw(uint32_t phase) {
  return -ComputeSampleRamp(phase);
}

int16_t Lfo::ComputeSampleRamp(uint32_t phase) {
  int16_t ramp = -32678 + (phase >> 16);
  uint32_t pi = phase_increment_ / divider_ * multiplier_ >> 16;
  int16_t x = 0;
  if (pi > kPI100Hz) {
    x = Interpolate1022(wav_saw100, phase);
  } else if (pi > kPI10Hz) {
    uint16_t balance = (pi - kPI10Hz) * 65535L / (kPI100Hz - kPI10Hz);
    x = Crossfade1022(wav_saw10, wav_saw100, phase, balance);
  } else if (pi > kPI1Hz) {
    uint16_t balance = (pi - kPI1Hz) * 65535L / (kPI10Hz - kPI1Hz);
    int32_t a = ramp;
    int32_t b = Interpolate1022(wav_saw10, phase);
    x = a + ((b - a) * static_cast<int32_t>(balance) >> 16);
  } else {
    x = ramp;
  }
  return x * level_ >> 16;
}

int16_t Lfo::ComputeSampleTrapezoid(uint32_t phase) {
  int16_t tri = phase < 1UL << 31 ? -32768 + (phase >> 15) :  32767 - (phase >> 15);
  int32_t trap = tri * 2;
  CONSTRAIN(trap, INT16_MIN, INT16_MAX);
  uint32_t pi = phase_increment_ / divider_ * multiplier_ >> 16;
  int16_t x = 0;
  if (pi > kPI100Hz) {
    x = Interpolate1022(wav_trap100, phase);
  } else if (pi > kPI10Hz) {
    uint16_t balance = (pi - kPI10Hz) * 65535L / (kPI100Hz - kPI10Hz);
    x = Crossfade1022(wav_trap10, wav_trap100, phase, balance);
  } else if (pi > kPI1Hz) {
    uint16_t balance = (pi - kPI1Hz) * 65535L / (kPI10Hz - kPI1Hz);
    int32_t a = trap;
    int32_t b = Interpolate1022(wav_trap10, phase);
    x = a + ((b - a) * static_cast<int32_t>(balance) >> 16);
  } else {
    x = trap;
  }
  return x * level_ >> 16;
}

int16_t Lfo::ComputeSampleSquare(uint32_t phase) {
  int16_t x;
  if (phase < UINT32_MAX / 2)
    x = INT16_MAX;
  else
    x = INT16_MIN >> 3;		// asymmetric square sounds better
				// when added in Quad mode.
  return x * level_ >> 16;
}

int16_t Lfo::ComputeSampleRandom(uint32_t phase, bool interpolation) {
  int16_t x;

  if (interpolation) {
    phase = Interpolate1022(wav_sine, (phase / 2) + UINT32_MAX / 4 * 3) + 32768;
    x = (next_value_ - current_value_) * phase / 65535 + current_value_;
  } else {
    x = current_value_;
  }

  return x * level_ >> 16;
}

}  // namespace batumi
