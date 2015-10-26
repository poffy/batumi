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
// Driver for the built-in ADC.

#include "drivers/adc.h"

#include <stm32f10x_conf.h>

namespace batumi {
  
void Adc::Init() {
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

  ADC_InitTypeDef adc_init;
  GPIO_InitTypeDef gpio_init;
  
  // adc pins configuration
  gpio_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &gpio_init);

  // demuxer pins configuration
  gpio_init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &gpio_init);

  ADC_DeInit(ADC1);
  ADC_DeInit(ADC2);
  adc_init.ADC_Mode = ADC_Mode_Independent;
  adc_init.ADC_ScanConvMode = DISABLE;
  adc_init.ADC_ContinuousConvMode = DISABLE;
  adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  adc_init.ADC_DataAlign = ADC_DataAlign_Left;
  adc_init.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &adc_init);
  ADC_Init(ADC2, &adc_init);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

  ADC_Cmd(ADC1, ENABLE);
  ADC_Cmd(ADC2, ENABLE);

  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1));
  ADC_ResetCalibration(ADC2);
  while (ADC_GetResetCalibrationStatus(ADC2));
  ADC_StartCalibration(ADC2);
  while (ADC_GetCalibrationStatus(ADC2));

  index_ = kNumAdcChannels - 1;
  last_read_ = 0;
  state_ = false;
  
  // read all current values by scanning completely once
  for (uint8_t i=0; i<kNumAdcChannels * 2; i++) {
    Scan();
    for(int j=0; j<2000; j++)	// wait a bit for the mux to catch up
      asm(""); // this is to avoid GCC's optimization
  }
}

void Adc::Scan() {
  if (state_) {
    // Read the value from the previous conversion.
    values1_[index_] = ADC1->DR - 32768;
    values2_[index_] = ADC2->DR - 32768;
    last_read_ = index_;
    ++index_;
    if (index_ >= kNumAdcChannels) {
      index_ = 0;
    }

    // Write the mux address.
    GPIO_WriteBit(GPIOA, GPIO_Pin_3, static_cast<BitAction>(index_ & 1));
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, static_cast<BitAction>(index_ & 2));
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, static_cast<BitAction>(index_ & 4));
  
  } else {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
  }
  state_ = !state_;
}

}  // namespace batumi
