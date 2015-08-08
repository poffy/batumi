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

#include "drivers/dac.h"

#include <stm32f10x_conf.h>

namespace batumi {

void Dac::Init() {
  
  // configure out pins
  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
    GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &gpio_init);

  // configure timers 3 and 4 for PWM
  TIM_TimeBaseInitTypeDef timer_init;
  TIM_TimeBaseStructInit(&timer_init);
  timer_init.TIM_Period = (1 << kPwmResolution) - 1;
  timer_init.TIM_Prescaler = 1;
  timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
  timer_init.TIM_CounterMode = TIM_CounterMode_Up;
  timer_init.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &timer_init);
  TIM_TimeBaseInit(TIM4, &timer_init);

  TIM_OCInitTypeDef output_compare;
  TIM_OCStructInit(&output_compare);
  output_compare.TIM_OCMode = TIM_OCMode_PWM1;
  output_compare.TIM_OutputState = TIM_OutputState_Enable;
  output_compare.TIM_Pulse = 0x00;
  output_compare.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM3, &output_compare);
  TIM_OC2Init(TIM3, &output_compare);
  TIM_OC3Init(TIM3, &output_compare);
  TIM_OC4Init(TIM3, &output_compare);
  TIM_OC1Init(TIM4, &output_compare);
  TIM_OC2Init(TIM4, &output_compare);
  TIM_OC3Init(TIM4, &output_compare);
  TIM_OC4Init(TIM4, &output_compare);

  for (int i=0; i<kNumDacChannels; i++)
    value_[i] = UINT16_MAX;
  Write();
}

void Dac::Write() {
  TIM_SetCompare1(TIM3, value_[0]);
  TIM_SetCompare2(TIM3, value_[1]);
  TIM_SetCompare3(TIM3, value_[2]);
  TIM_SetCompare4(TIM3, value_[3]);
  TIM_SetCompare3(TIM4, value_[4]);
  TIM_SetCompare2(TIM4, value_[5]);
  TIM_SetCompare1(TIM4, value_[6]);
  TIM_SetCompare4(TIM4, value_[7]);
}

}  // namespace batumi
