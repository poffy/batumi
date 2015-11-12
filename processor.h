// Copyright 2015 Matthias Puech
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
// Processor. Orchestrates the four LFOs.

#ifndef BATUMI_MODULATIONS_PROCESSOR_H_
#define BATUMI_MODULATIONS_PROCESSOR_H_

#include "drivers/adc.h"
#include "drivers/dac.h"

#include "lfo.h"
#include "ui.h"

namespace batumi {

const uint8_t kNumChannels = 4;

class Processor {
public:

  Processor() { }
  ~Processor() { }

  void Init(Ui *ui, Adc *adc, Dac *dac);
  void Process();

private:
  Lfo lfo_[kNumChannels];
  Ui *ui_;
  Adc *adc_;
  Dac *dac_;

  FeatureMode previous_feat_mode_;

  bool reset_trigger_armed_[kNumChannels];
  bool reset_triggered_[kNumChannels];
  uint8_t reset_subsample_[kNumChannels];
  uint32_t last_reset_[kNumChannels];
  int16_t previous_reset_[kNumChannels];
  int16_t last_pitch_[kNumChannels];
  bool synced_[kNumChannels];
  int16_t filtered_cv_[kNumChannels];
  uint8_t waveform_offset_;

  void SetFrequency(int8_t lfo_no);

  DISALLOW_COPY_AND_ASSIGN(Processor);
};

}  // namespace batumi

#endif  // BATUMI_MODULATIONS_LFO_H_
