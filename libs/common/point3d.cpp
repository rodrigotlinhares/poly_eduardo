#include "point3d.h"

////// PUBLIC METHODS //////

Point3D::Point3D(void)
  : m_x(0),
    m_y(0),
    m_z(0)
{
  
}

Point3D::Point3D(float x, float y, float z)
  : m_x(x),
    m_y(y),
    m_z(z)
{
  
}

float Point3D::x(void) const
{
  return m_x;
}

float Point3D::y(void) const
{
  return m_y;
}

float Point3D::z(void) const
{
  return m_z;
}

Point3D& Point3D::operator +=(const Point3D& p)
{
  m_x += p.x();
  m_y += p.y();
  m_z += p.z();

  return *this;
}

Point3D& Point3D::operator -=(const Point3D& p)
{
  m_x -= p.x();
  m_y -= p.y();
  m_z -= p.z();

  return *this;
}

Point3D& Point3D::operator *=(const Point3D& p)
{
  m_x *= p.x();
  m_y *= p.y();
  m_z *= p.z();

  return *this;
}

Point3D& Point3D::operator *=(float f)
{
  m_x *= f;
  m_y *= f;
  m_z *= f;

  return *this;
}

Point3D Point3D::operator +(const Point3D& p) const
{
  return Point3D(*this) += p;
}

Point3D Point3D::operator -(const Point3D& p) const
{
  return Point3D(*this) -= p;
}

Point3D Point3D::operator *(const Point3D& p) const
{
  return Point3D(*this) *= p;
}

Point3D Point3D::operator *(const float f) const
{
  return Point3D(*this) *= f;
}

float& Point3D::operator [](uint32_t index)
{
  assert(index < 3);
  return (&m_x)[index];
}

const float& Point3D::operator [](uint32_t index) const
{
  assert(index < 3);
  return (&m_x)[index];
}
