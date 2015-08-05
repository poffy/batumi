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
// Driver for "fake" DAC (PWM)

#ifndef BATUMI_DRIVERS_DAC_H_
#define BATUMI_DRIVERS_DAC_H_

#include "stmlib/stmlib.h"

const uint8_t kNumDacChannels = 8;
const uint16_t kPwmResolution = 12;  // bits

namespace batumi {

class Dac {
 public:
  Dac() { }
  ~Dac() { }
  
  void Init();

  inline void set(uint8_t channel, int16_t value) {
    value_[channel] = (32768 - value) >> (16 - kPwmResolution);
  }

  inline void set_sine(uint8_t channel, int16_t value) { set(channel, value); }
  inline void set_asgn(uint8_t channel, int16_t value) { set(channel+4, value); }

  void Write();
  
 private:
  uint16_t value_[kNumDacChannels];

  DISALLOW_COPY_AND_ASSIGN(Dac);
};

}  // namespace batumi

#endif  // BATUMI_DRIVERS_DAC_H_
