#include "filtersbasis.h"

void FILTERS::FiltersBasis::setSize()
{
  m_size = m_width * m_height;
}

int FILTERS::FiltersBasis::width()
{
  return m_width;
}

int FILTERS::FiltersBasis::height()
{
  return m_height;
}

int FILTERS::FiltersBasis::size()
{
  return m_width;
}

bool* FILTERS::FiltersBasis::data()
{
  return m_buffer;
}

void FILTERS::FiltersBasis::setWidth(int width)
{
  m_width = width;
  setSize();
}

void FILTERS::FiltersBasis::setHeight(int height)
{
  m_height = height;
  setSize();
}

void FILTERS::FiltersBasis::setFrameSize(int width, int height)
{
  m_width = width;
  m_height = height;
  setSize();
}
