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
// Resources definitions.
//
// Automatically generated with:
// make resources


#ifndef _RESOURCES_H_
#define _RESOURCES_H_


#include "stmlib/stmlib.h"



namespace batumi {

typedef uint8_t ResourceId;

extern const char* string_table[];

extern const uint16_t* lookup_table_table[];

extern const uint32_t* lookup_table_32_table[];

extern const int16_t* waveform_table[];

extern const uint32_t lut_increments[];
extern const uint32_t lut_lfo_increments[];
extern const int16_t wav_sine[];
extern const int16_t wav_saw10[];
extern const int16_t wav_saw100[];
extern const int16_t wav_tri10[];
extern const int16_t wav_tri100[];
extern const int16_t wav_trap10[];
extern const int16_t wav_trap100[];
#define STR_DUMMY 0  // dummy
#define LUT_INCREMENTS 0
#define LUT_INCREMENTS_SIZE 97
#define LUT_LFO_INCREMENTS 1
#define LUT_LFO_INCREMENTS_SIZE 1025
#define WAV_SINE 0
#define WAV_SINE_SIZE 1025
#define WAV_SAW10 1
#define WAV_SAW10_SIZE 1025
#define WAV_SAW100 2
#define WAV_SAW100_SIZE 1025
#define WAV_TRI10 3
#define WAV_TRI10_SIZE 1025
#define WAV_TRI100 4
#define WAV_TRI100_SIZE 1025
#define WAV_TRAP10 5
#define WAV_TRAP10_SIZE 1025
#define WAV_TRAP100 6
#define WAV_TRAP100_SIZE 1025

}  // namespace batumi

#endif  // _RESOURCES_H_
