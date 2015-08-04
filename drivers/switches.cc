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
// Driver for the split and function switches.

#include "drivers/switches.h"

#include <string.h>

namespace batumi {

void Switches::Init(Adc *adc) {
  adc_ = adc;

  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_IPU;

  gpio_init.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &gpio_init);
}

void Switches::Debounce() {
  switch_state_[0] = (switch_state_[0] << 1) |
    !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
  switch_state_[1] = (switch_state_[1] << 1) |
    !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
  switch_state_[2] = (switch_state_[2] << 1) |
    !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
  switch_state_[3] = (switch_state_[3] << 1) |
    (adc_->value(ADC_TACT_SWITCH) > 0);

}

}  // namespace batumi
