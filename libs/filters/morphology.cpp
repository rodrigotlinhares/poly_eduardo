#include "morphology.h"

FILTERS::Morphology::Morphology()
{
  m_buffer = NULL;
  m_data = NULL;
  m_width = 0;
  m_height = 0;
  m_size = 0;
}

FILTERS::Morphology::~Morphology()
{
  if (m_buffer != NULL)
    free(m_buffer);
  if (m_data != NULL)
    free(m_data);
}

bool* FILTERS::Morphology::data()
{
  return m_data;
}

void FILTERS::Morphology::initialize()
{
  m_buffer = (bool*) calloc(m_size, sizeof(bool));
  m_data = (bool*) calloc(m_size, sizeof(bool));
}

void FILTERS::Morphology::erode(bool* data, FILTERS::Morphology::Kernel kernel)
{
  FILTERS::Morphology::erode_dilation(data, kernel, 0);
}

void FILTERS::Morphology::dilation(bool* data, FILTERS::Morphology::Kernel kernel)
{
  FILTERS::Morphology::erode_dilation(data, kernel, 1);
}

void FILTERS::Morphology::erode_dilation(bool* data, FILTERS::Morphology::Kernel kernel, bool filter)
{
  register bool check = filter ? true : false;
  register bool set = filter ? false : true;

  memset(m_buffer, check, m_size * sizeof(bool));

  register bool* d_last_line = m_buffer + m_size - m_width - 1;
  register bool* dest = m_buffer + m_width + 1;
  register const bool* p1 = data;
  register const bool* p2 = p1 + 1;
  register const bool* p3 = p2 + 1;
  register const bool* p4 = p1 + m_width;
  register const bool* p5 = p4 + 1;
  register const bool* p6 = p5 + 1;
  register const bool* p7 = p4 + m_width;
  register const bool* p8 = p7 + 1;
  register const bool* p9 = p8 + 1;
  register const bool* last = p1 + m_size - 1;

  if (check)
  {
    memset(m_buffer, false, (m_width + 1) * sizeof(bool));
    memset(d_last_line, false, (m_width + 1) * sizeof(bool));
  }

  switch (kernel)
  {
  case FILTERS::Morphology::Kernel_Vertical:
    for (;;)
    {
      if (p8 == last)
        break;

      if (*p5 != check &&
          *p2 != check &&
          *p8 != check)
        *dest = set;

      p2++; p5++; p8++; dest++;
    }
    break;
  case FILTERS::Morphology::Kernel_Horizontal:
    for (;;)
    {
      if (p6 == last)
        break;

      if (*p5 != check &&
          *p4 != check &&
          *p6 != check)
        *dest = set;

      p4++; p5++; p6++; dest++;
    }
    break;
  case FILTERS::Morphology::Kernel_Neighborhood_4:
    for (;;)
    {
      if (p8 == last)
        break;
      
      if (*p5 != check &&
          *p2 != check &&
          *p4 != check &&
          *p6 != check &&
          *p8 != check)
        *dest = set;

      p2++; p4++; p5++; p6++; p8++; dest++;
    }
    break;
  case FILTERS::Morphology::Kernel_Neighborhood_8:
    for (;;)
    {
      if (*p5 != check &&
          *p1 != check &&
          *p2 != check &&
          *p3 != check &&
          *p4 != check &&
          *p6 != check &&
          *p7 != check &&
          *p8 != check &&
          *p9 != check)
        *dest = set;

      if (p9 == last)
        break;

      p1++; p2++; p3++; p4++; p5++; p6++; p7++; p8++; p9++; dest++;
    }
    break;
  }

  memcpy(m_data, m_buffer, m_size * sizeof(bool));
}
