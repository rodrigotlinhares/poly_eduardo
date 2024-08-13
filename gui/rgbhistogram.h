#ifndef RGB_HISTOGRAM_H
#define RGB_HISTOGRAM_H

#include <QtOpenGL>
#include <QColor>
#include <QPointF>
#include <QList>
#include <cstdlib>
#include <cstdint>
#include <omp.h>
#include <common/point3d.h>

class RGBHistogram : public QGLWidget
{
  Q_OBJECT;

 public:
  RGBHistogram(QWidget* parent = 0);
  ~RGBHistogram(void);

 private:
  void resizeGL(int width, int height);
  void paintGL(void);

  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  void drawCube(void);
  void drawHistogram(void);

 private slots:
  void updateCube(uint8_t* data);
  void updateSize(int width, int height);

 private:
  QColor m_bg;
  QPointF m_last_pos;
  Point3D m_pos;
  float m_rotation;
  int m_size;
  uint8_t* m_data;
  GLfloat* m_points;
  GLfloat m_cube_vertices[24];
  GLubyte m_cube_indices[24];
};

#endif
