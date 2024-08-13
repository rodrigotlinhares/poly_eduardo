#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "filtersbasis.h"

#include <cstdlib>
#include <string.h>

namespace FILTERS
{
  class Morphology : public FiltersBasis
  {
  public:
    Morphology();
    ~Morphology();

    enum Kernel
    {
      Kernel_Vertical,
      Kernel_Horizontal,
      Kernel_Neighborhood_4,
      Kernel_Neighborhood_8
    };
    
    bool* data();

    void initialize();

    void erode(bool* data, Kernel kernel);
    void dilation(bool* data, Kernel kernel);
    
  private:
    bool* m_data;

    void erode_dilation(bool* data, Kernel kernel, bool filter);
  };
}

#endif
