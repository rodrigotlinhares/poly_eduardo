#include "conversion.h"

////// PUBLIC METHODS //////

FILTERS::Conversion::Conversion(void)
  : m_data(NULL)
{
  m_width = m_height = m_size = 0;
  m_buffer = NULL;
}

FILTERS::Conversion::~Conversion(void)
{
  if (m_buffer != NULL)
    free(m_buffer);
  if (m_data != NULL)
    free(m_data);
}

uint8_t* FILTERS::Conversion::charData(void)
{
  return m_data;
}

void FILTERS::Conversion::initialize(void)
{
  m_buffer = (bool*) calloc(m_size, sizeof(bool));
  m_data = (uint8_t*) calloc(m_size * 4, sizeof(uint8_t));
}

void FILTERS::Conversion::convert(void* data, FromTo from_to)
{
  switch (from_to)
  {
  case FILTERS::Conversion::Bin2Gray:
    bin2gray((bool*) data);
    break;
  }
}

////// PRIVATE METHODS //////

void FILTERS::Conversion::bin2gray(bool* data)
{
  register const bool* binary = data;
  register const bool* last = data + m_size - 1;
  register uint8_t* r = m_data;
  register uint8_t* g = r + 1;
  register uint8_t* b = g + 1;
  register uint8_t* a = b + 1;

  for (;;)
  {
    if (*binary)
      *r = *g = *b = *a = 255;
    else
    {
      *r = *g = *b = 0;
      *a = 255;
    }

    if (binary == last)
      break;
      
    binary++; r += 4; g += 4; b += 4; a += 4;
  }
}
