#ifndef CONVERSION_H
#define CONVERSION_H

#include <cstdlib>
#include <cstdint>

#include "filtersbasis.h"

namespace FILTERS
{
 class Conversion : public FiltersBasis
  {
  public:
    Conversion(void);
    ~Conversion(void);

    enum FromTo
    {
      Bin2Gray
    };

    uint8_t* charData(void);
    
    void initialize(void);
    void convert(void* data, FromTo from_to);

  private:
    void bin2gray(bool* data);
    
  private:
    uint8_t* m_data;
  };
}

#endif
