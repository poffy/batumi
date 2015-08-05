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
LFO and envelope increments.
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


min_frequency = 5  # Hertz
max_frequency = min_frequency * 100  # Hertz

excursion = 1 << 32
num_values = 1025
min_increment = excursion * min_frequency / sample_rate
max_increment = excursion * max_frequency / sample_rate

rates = numpy.linspace(numpy.log(min_increment),
                       numpy.log(max_increment), num_values)
lookup_tables_32.append(
    ('lfo_increments', numpy.exp(rates).astype(int))
)
