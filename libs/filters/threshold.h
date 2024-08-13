#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <cstdlib>
#include <cstdint>

#include "filtersbasis.h"

namespace FILTERS
{
 class Threshold : public FiltersBasis
  {
  public:
    Threshold(void);
    ~Threshold(void);

    void initialize(void);

    void threshold(uint8_t* data, uint32_t threshold);
  };
}

#endif
