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

#include "lfo.h"

namespace batumi {

const uint8_t kFinePotDivider = 8;

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
  UI_MODE_ZOOM,
  UI_MODE_CATCHUP,
};

class Ui {
 public:
  Ui() { }
  ~Ui() { }
  
  void Init(Adc *adc, Lfo lfo[4]);
  void Poll();
  void DoEvents();
  void FlushEvents();

  uint16_t pot(uint8_t channel) {
    int32_t pot = pot_coarse_value_[channel]  +
      (pot_fine_value_[channel] - 32768) / 8;

    CONSTRAIN(pot, 0, UINT16_MAX);
    return pot;
  }

  inline FeatureMode feat_mode() const { return feat_mode_; }
  inline UiMode mode() const { return mode_; }
  inline LfoShape shape() const {
    return static_cast<LfoShape>((switches_.pressed(2) << 1) |
				 switches_.pressed(1));
  }

 private:
  void OnSwitchPressed(const stmlib::Event& e);
  void OnSwitchReleased(const stmlib::Event& e);
  void OnPotChanged(const stmlib::Event& e);
  void SyncWithPots();

  uint16_t pot_value_[4];
  uint16_t pot_filtered_value_[4];
  uint16_t pot_coarse_value_[4];
  uint16_t pot_fine_value_[4];
  uint32_t press_time_[kNumSwitches];
  bool detect_very_long_press_[kNumSwitches];
  bool catchup_state_[4];

  stmlib::EventQueue<32> queue_;

  Leds leds_;
  Switches switches_;
  Adc *adc_;
  Lfo *lfo_;

  UiMode mode_;
  FeatureMode feat_mode_;

  DISALLOW_COPY_AND_ASSIGN(Ui);
};

}  // namespace batumi

#endif  // BATUMI_UI_H_
