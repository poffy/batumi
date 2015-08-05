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
// User interface.

#include "ui.h"

#include <algorithm>

namespace batumi {

using namespace std;
using namespace stmlib;

const int32_t kLongPressDuration = 800;
const int32_t kVeryLongPressDuration = 2000;
const uint16_t kAdcThreshold = 1 << (16 - 10);  // 10 bits

int32_t animation_counter_ = 0;

void Ui::Init(Adc *adc, Lfo lfo[]) {
  mode_ = UI_MODE_SPLASH;
  lfo_ = lfo;
  feat_mode_ = FEAT_MODE_FREE;
  adc_ = adc;
  leds_.Init();
  switches_.Init(adc_);
}

void Ui::Poll() {
  switches_.Debounce();
  
  for (uint8_t i = 0; i < kNumSwitches; ++i) {
    if (switches_.just_pressed(i)) {
      queue_.AddEvent(CONTROL_SWITCH, i, 0);
      press_time_[i] = system_clock.milliseconds();
      detect_very_long_press_[i] = false;
    }
    if (switches_.pressed(i)) {
      int32_t pressed_time = system_clock.milliseconds() - press_time_[i];

      if (!detect_very_long_press_[i]) {
        if (pressed_time > kLongPressDuration) {
          queue_.AddEvent(CONTROL_SWITCH, i, pressed_time);
          detect_very_long_press_[i] = true;
        }
      } else {
        if (pressed_time > kVeryLongPressDuration) {
          queue_.AddEvent(CONTROL_SWITCH, i, pressed_time);
          detect_very_long_press_[i] = false;
          press_time_[i] = 0;
        }
      }
    }
    
    if (switches_.released(i) &&
        press_time_[i] != 0 &&
        !detect_very_long_press_[i]) {
      queue_.AddEvent(
          CONTROL_SWITCH,
          i,
          system_clock.milliseconds() - press_time_[i] + 1);
      press_time_[i] = 0;
      detect_very_long_press_[i] = false;
    }
  }
  
  for (uint8_t i = 0; i <= kNumAdcChannels; ++i) {
    int32_t value = (31 * adc_filtered_value_[i] + adc_->value(i)) >> 5;
    adc_filtered_value_[i] = value;
    int32_t current_value = static_cast<int32_t>(adc_value_[i]);
    if (value >= current_value + kAdcThreshold ||
        value <= current_value - kAdcThreshold) {
      queue_.AddEvent(CONTROL_POT, i, value);
      adc_value_[i] = value;
    }
  }
  
  switch (mode_) {
  case UI_MODE_SPLASH:
    animation_counter_++;
    if (animation_counter_ % 64 == 0) {
      for (int i=0; i<kNumLeds; i++)
	leds_.set(i, ((animation_counter_ / 64) % 4) == i);
      if (animation_counter_ / 64 > 8)
	mode_ = UI_MODE_NORMAL;
    }
    break;

  case UI_MODE_NORMAL:
    for (uint8_t i=0; i<kNumLeds; i++)
      leds_.set(i, false);
    leds_.set(feat_mode_, true);
    break;

  case UI_MODE_ZOOM:
    animation_counter_++;
    for (uint8_t i=0; i<kNumLeds; i++)
      leds_.set(i, false);
    leds_.set(feat_mode_, animation_counter_ & 128);
    break;
  }

  leds_.Write();
}

void Ui::FlushEvents() {
  queue_.Flush();
}

void Ui::OnSwitchPressed(const Event& e) {
}

void Ui::OnSwitchReleased(const Event& e) {
  switch (e.control_id) {
  case SWITCH_SYNC:
  case SWITCH_WAV1:
  case SWITCH_WAV2:
    break;
  case SWITCH_SELECT:
    if (e.data > kVeryLongPressDuration) {
    } else if (e.data > kLongPressDuration) {
      mode_ = UI_MODE_ZOOM;
    } else {
      switch (mode_) {
      case UI_MODE_SPLASH:
	break;
      case UI_MODE_ZOOM:
	mode_ = UI_MODE_NORMAL;
	break;
      case UI_MODE_NORMAL:
	feat_mode_ = static_cast<FeatureMode>((feat_mode_ + 1) % FEAT_MODE_LAST);
	for (int i=0; i<4; i++)
	  lfo_[i].Init();
	break;
      }
    }
    break;
  }
}

void Ui::OnPotChanged(const Event& e) {
}

void Ui::DoEvents() {
  while (queue_.available()) {
    Event e = queue_.PullEvent();
    if (e.control_type == CONTROL_SWITCH) {
      if (e.data == 0) {
        OnSwitchPressed(e);
      } else {
        OnSwitchReleased(e);
      }
    } else if (e.control_type == CONTROL_POT) {
      OnPotChanged(e);
    }
  }
  if (queue_.idle_time() > 500) {
    queue_.Touch();
  }
}

}  // namespace batumi
