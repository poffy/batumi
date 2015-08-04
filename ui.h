// Copyright 2013 Matthias Puech.
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
// Settings

#ifndef BATUMI_UI_H_
#define BATUMI_UI_H_

#include "stmlib/stmlib.h"

#include "stmlib/ui/event_queue.h"

#include "drivers/adc.h"
#include "drivers/leds.h"
#include "drivers/switches.h"

namespace batumi {

enum FeatureMode {
  FEAT_MODE_FREE,
  FEAT_MODE_QUAD,
  FEAT_MODE_PHASE,
  FEAT_MODE_DIVIDE,
  FEAT_MODE_LAST
};

enum UiMode {
  UI_MODE_SPLASH,
  UI_MODE_NORMAL,
};

class Ui {
 public:
  Ui() { }
  ~Ui() { }
  
  void Init(Adc *adc);
  void Poll();
  void DoEvents();
  void FlushEvents();

  inline UiMode mode() const { return mode_; }

 private:
  void OnSwitchPressed(const stmlib::Event& e);
  void OnSwitchReleased(const stmlib::Event& e);
  void OnPotChanged(const stmlib::Event& e);

  uint16_t adc_value_[kNumChannels];
  uint16_t adc_filtered_value_[kNumChannels];
  uint32_t press_time_[kNumSwitches];
  bool detect_very_long_press_[kNumSwitches];

  stmlib::EventQueue<32> queue_;

  Leds leds_;
  Switches switches_;
  Adc *adc_;

  UiMode mode_;
  FeatureMode feat_mode_;

  DISALLOW_COPY_AND_ASSIGN(Ui);
};

}  // namespace batumi

#endif  // BATUMI_UI_H_
