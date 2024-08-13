#ifndef FILTERS_BASIS_H
#define FILTERS_BASIS_H

namespace FILTERS
{
  class FiltersBasis
  {
  protected:
    int m_width;
    int m_height;
    int m_size;
    bool* m_buffer;

    void setSize();
    
  public:
    int width();
    int height();
    int size();
    bool* data();

    void setWidth(int width);
    void setHeight(int height);
    void setFrameSize(int width, int height);
  };
}

#endif
