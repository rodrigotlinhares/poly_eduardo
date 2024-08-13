#include "threshold.h"

////// PUBLIC METHODS //////

FILTERS::Threshold::Threshold(void)
{
  m_width = m_height = m_size = 0;
  m_buffer = NULL;
}

FILTERS::Threshold::~Threshold(void)
{
  if (m_buffer != NULL)
    free(m_buffer);
}

void FILTERS::Threshold::initialize(void)
{
  m_buffer = (bool*) calloc(m_size, sizeof(bool));
}

void FILTERS::Threshold::threshold(uint8_t* data, uint32_t threshold)
{
  register const uint8_t* r = data;
  register const uint8_t* g = r + 1;
  register const uint8_t* b = g + 1;
  register const uint8_t* a = b + 1;
  register const uint8_t* last = r + m_size * 4 - 1;
  register bool* dest = m_buffer;
  register uint8_t median;

  for (;;)
  {
    median = (*r + *g + *b + *a) * 0.25;
    *dest = median >= threshold ? true : false;
    
    if (a == last)
      break;

    r += 4; g += 4; b += 4; a += 4; dest++;
  }
}
