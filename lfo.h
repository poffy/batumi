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

namespace batumi {

const int16_t kOctave = 12 * 128;

/* phase increment values for given frequencies */
const uint32_t kPI1Hz = UINT16_MAX / SAMPLE_RATE * 1;
const uint32_t kPI10Hz = UINT16_MAX / SAMPLE_RATE * 10;
const uint32_t kPI100Hz = UINT16_MAX / SAMPLE_RATE * 100;

enum LfoShape {
  SHAPE_TRAPEZOID,
  SHAPE_RAMP,
  SHAPE_SAW,
  SHAPE_TRIANGLE,
};

class Lfo {
 public:
   
  Lfo() { }
  ~Lfo() { }
  
  void Init();
  void Step();

  inline void set_pitch(int16_t pitch) {
    phase_increment_ = ComputePhaseIncrement(pitch);
  };

  inline void set_period(uint32_t period) {
    phase_increment_ = UINT32_MAX / period;
  }

  inline void set_phase(uint16_t phase) {
    initial_phase_ = phase << 16;
  }

  inline void set_divider(uint16_t divider) {
    divider_ = divider;
  }

  inline void set_level(uint16_t level) {
    level_ = level;
  }

  inline void Reset() {
    phase_ = 0;
    divider_counter_ = 0;
  }

  int16_t ComputeSampleShape(LfoShape s);
  int16_t ComputeSampleSine();
  int16_t ComputeSampleTriangle();
  int16_t ComputeSampleTrapezoid();
  int16_t ComputeSampleRamp();
  int16_t ComputeSampleSaw();

 private:

  uint32_t ComputePhaseIncrement(int16_t pitch);
  int16_t pitch_;
  uint32_t phase_, divided_phase_;
  uint16_t divider_, divider_counter_;
  uint16_t level_;
  uint32_t initial_phase_;
  uint32_t phase_increment_;

  DISALLOW_COPY_AND_ASSIGN(Lfo);
};

}  // namespace batumi

#endif  // BATUMI_MODULATIONS_LFO_H_
