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

#ifndef BATUMI_MODULATIONS_LFO_H_
#define BATUMI_MODULATIONS_LFO_H_

#include "stmlib/stmlib.h"
#include "resources.h"

namespace batumi {

const int16_t kOctave = 12 * 128;

/* phase increment values for given frequencies */
const uint32_t kPI1Hz = UINT16_MAX / SAMPLE_RATE * 1;
const uint32_t kPI10Hz = UINT16_MAX / SAMPLE_RATE * 10;
const uint32_t kPI100Hz = UINT16_MAX / SAMPLE_RATE * 100;

enum LfoShape {
  SHAPE_SINE,
  SHAPE_TRAPEZOID,
  SHAPE_RAMP,
  SHAPE_SAW,
  SHAPE_TRIANGLE,
  SHAPE_SQUARE,
  SHAPE_RANDOM_STEP,
  SHAPE_RANDOM_SMOOTH,
  SHAPE_LOGISTIC_STEP,
  SHAPE_LOGISTIC_SMOOTH,
};

const uint8_t kNumLfoShapes = 5;

class Lfo {
 public:
   
  Lfo() { }
  ~Lfo() { }
  
  void Init();
  void Step();

  inline void set_pitch(int16_t pitch) {
    if (pitch == INT16_MIN)
      phase_increment_ = 0;
    else
      phase_increment_ = ComputePhaseIncrement(pitch);
  };

  inline void set_period(uint32_t period) {
    phase_increment_ = UINT32_MAX / period;
  }

  inline void set_initial_phase(uint16_t phase) {
    initial_phase_ = phase << 16;
  }

  inline void set_divider(uint16_t divider) {
    divider_ = divider;
    divider_counter_ = cycle_counter_ % divider_;
  }

  inline void set_multiplier(uint16_t mult) {
    multiplier_ = mult;
  }

  inline void set_level(uint16_t level) {
    level_ = level;
  }

  inline uint16_t level() {
    return level_;
  }

  void Reset(uint8_t subsample);

  inline void link_to(Lfo *lfo) {
    phase_ = lfo->phase_;
    phase_increment_ = lfo->phase_increment_;
  }

  int16_t ComputeSampleShape(LfoShape s);
  int16_t ComputeSampleSine(uint32_t phase);
  int16_t ComputeSampleTriangle(uint32_t phase);
  int16_t ComputeSampleTrapezoid(uint32_t phase);
  int16_t ComputeSampleRamp(uint32_t phase);
  int16_t ComputeSampleSaw(uint32_t phase);
  int16_t ComputeSampleSquare(uint32_t phase);
  int16_t ComputeSampleRandom(uint32_t phase, bool interpolation);

 private:

  inline uint32_t phase() {
    return multiplied_phase_ + initial_phase_;
  }

  int16_t ComputeSampleShape(LfoShape s, uint32_t phase);

  uint32_t ComputePhaseIncrement(int16_t pitch);
  uint32_t phase_, divided_phase_, multiplied_phase_;
  uint16_t divider_, divider_counter_, cycle_counter_;
  uint16_t multiplier_;
  uint16_t level_;
  uint32_t initial_phase_;
  uint32_t phase_increment_;
  uint16_t bl_step_counter_;
  uint8_t reset_subsample_;

  /* values of the oscillators for each shape before and
   * after reset */
  int16_t step_begin_[kNumLfoShapes];
  int16_t step_end_[kNumLfoShapes];

  /* random waveshapes */
  int16_t next_value_;
  int16_t current_value_;


  enum RandomType {
    RANDOM_WHITE,
    RANDOM_LOGISTIC,
  };

  RandomType random_type_;


  DISALLOW_COPY_AND_ASSIGN(Lfo);
};

}  // namespace batumi

#endif  // BATUMI_MODULATIONS_LFO_H_
