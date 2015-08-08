#ifndef BATUMI_MODULATIONS_PROCESSOR_H_
#define BATUMI_MODULATIONS_PROCESSOR_H_

#include "drivers/adc.h"
#include "drivers/dac.h"

#include "lfo.h"
#include "ui.h"

namespace batumi {

const uint8_t kNumChannels = 4;

class Processor {
public:

  Processor() { }
  ~Processor() { }

  void Init(Ui *ui, Adc *adc, Dac *dac);
  void Process();

private:
  Lfo lfo_[kNumChannels];
  Ui *ui_;
  Adc *adc_;
  Dac *dac_;

  FeatureMode previous_feat_mode_;

  uint16_t filtered_reset_[kNumChannels];
  bool reset_trigger_armed_[kNumChannels];
  uint32_t last_reset_[kNumChannels];

  DISALLOW_COPY_AND_ASSIGN(Processor);
};

}  // namespace batumi

#endif  // BATUMI_MODULATIONS_LFO_H_
