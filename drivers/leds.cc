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
// Driver for the status LEDs.

#include "drivers/leds.h"

#include <stm32f10x_conf.h>

namespace batumi {

void Leds::Init() {
  GPIO_InitTypeDef gpio_init;
  GPIO_StructInit(&gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_2;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &gpio_init);

  for (int i=0; i<kNumLeds; i++)
    values_[i] = 0;
  Write();
}

void Leds::Write() {
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, static_cast<BitAction>(values_[0]));
  GPIO_WriteBit(GPIOC, GPIO_Pin_14, static_cast<BitAction>(values_[1]));
  GPIO_WriteBit(GPIOC, GPIO_Pin_15, static_cast<BitAction>(values_[2]));
  GPIO_WriteBit(GPIOA, GPIO_Pin_2, static_cast<BitAction>(values_[3]));
}

}  // namespace batumi
