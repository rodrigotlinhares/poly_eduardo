#ifndef POINT3D_H
#define POINT3D_H

#include <cassert>
#include <cstdint>

class Point3D
{
 public:
  Point3D(void);
  Point3D(float x, float y, float z);

  float x(void) const;
  float y(void) const;
  float z(void) const;

  Point3D& operator +=(const Point3D& p);
  Point3D& operator -=(const Point3D& p);
  Point3D& operator *=(const Point3D& p);
  Point3D& operator *=(const float f);
  Point3D operator +(const Point3D& p) const;
  Point3D operator -(const Point3D& p) const;
  Point3D operator *(const Point3D& p) const;
  Point3D operator *(const float f) const;
  float& operator [](uint32_t index);
  const float& operator [](uint32_t index) const;

 private:
  float m_x, m_y, m_z;
};

#endif
