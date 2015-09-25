#!/usr/bin/python2.5
#
# Copyright 2014 Olivier Gillet.
#
# Author: Olivier Gillet (ol.gillet@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See http://creativecommons.org/licenses/MIT/ for more information.
#
# -----------------------------------------------------------------------------
#
# Lookup table definitions.

import numpy
import os

sample_rate = int(os.environ["SAMPLE_RATE"])

"""----------------------------------------------------------------------------
Pitch increments
----------------------------------------------------------------------------"""

lookup_tables = []
lookup_tables_32 = []

excursion = float(1 << 32)

a4_midi = 69
a4_pitch = 440.0
notes = numpy.arange(0 * 128.0, 12 * 128.0 + 16, 16) / 128.0
pitches = a4_pitch * 2 ** ((notes - a4_midi) / 12)
increments = excursion / sample_rate * pitches

lookup_tables_32.append(
    ('increments', increments.astype(int)))


"""----------------------------------------------------------------------------
Rough fader scaler (from hand-taken measurements)
----------------------------------------------------------------------------"""

# measurements taken at each marks on the panel
fader_scale6 = [0, 6637, 23378, 40134, 57125, 65520]
fader_scale5 = [0, 9972, 31880, 52396, 65520]

import math
from scipy import interpolate

x = numpy.arange(0, 65535, 255)

# scaler for frequency selection
def freq_to_midi(freq):
    return (12.0 * math.log(freq/440.0, 2) + 69.0)

scale_pitch = map(freq_to_midi, [0.01, 0.05, 0.5, 2.5, 15, 106])
scale_pitch = [(z * 128) + 32768 for z in scale_pitch]
scale_pitch_fun = interpolate.interp1d(fader_scale6, scale_pitch, kind='linear')
lookup_tables.append(('scale_pitch', scale_pitch_fun(x)))

# scaler for phase selection
step = 65536 / 4
scale_phase = [(step*0), (step*1)-1, (step*2)-1, (step*3)-1, (step*4)-1]
scale_phase_fun = interpolate.interp1d(fader_scale5, scale_phase, kind='linear')
lookup_tables.append(('scale_phase', scale_phase_fun(x)))

# scaler for divider
scale_divide = [32, 16, 8, 4, 3, 2]
scale_divide_fun = interpolate.interp1d(fader_scale6, scale_divide, kind='nearest')
lookup_tables.append(('scale_divide', scale_divide_fun(x)))
