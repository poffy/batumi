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
// Driver for ADC.

#ifndef BATUMI_DRIVERS_ADC_H_
#define BATUMI_DRIVERS_ADC_H_

#include "stmlib/stmlib.h"

namespace batumi {

const uint8_t kNumAdcChannels = 8;

enum AdcChannel {
  ADC_CV1,
  ADC_CV2,
  ADC_CV3,
  ADC_CV4,
  ADC_RESET1,
  ADC_RESET2,
  ADC_RESET3,
  ADC_RESET4,
  ADC_POT1,
  ADC_POT2,
  ADC_POT3,
  ADC_POT4,
  ADC_TACT_SWITCH,
};

class Adc {
 public:
  Adc() { }
  ~Adc() { }
  
  void Init();
  void Scan();

  inline int16_t value(uint8_t i) const {
    if (i<8) return values1_[i];
    else return values2_[i-8];
  }

  inline int16_t cv(uint8_t i) const {
    return value(ADC_CV1+i);
  }

  inline int16_t reset(uint8_t i) const {
    return value(ADC_RESET1+i);
  }

  inline uint16_t pot(uint8_t i) const {
    return value(ADC_POT1+i) + 32768;
  }


 private:
  int16_t values1_[kNumAdcChannels];
  int16_t values2_[kNumAdcChannels];

  bool state_;
  uint8_t index_;
  uint8_t last_read_;

  DISALLOW_COPY_AND_ASSIGN(Adc);
};

}  // namespace batumi

#endif  // BATUMI_DRIVERS_ADC_H_
