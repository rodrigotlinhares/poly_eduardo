#include "rgbhistogram.h"

////// PUBLIC METHODS //////

RGBHistogram::RGBHistogram(QWidget* parent)
  : QGLWidget(parent),
    m_bg(51, 51, 51),
    m_last_pos(0.0f, 0.0f),
    m_pos(0.0f, 0.0f, 5.0f),
    m_rotation(0.0f),
    m_data(NULL),
    m_points(NULL)
{
  m_cube_vertices[0] = 0.0f; m_cube_vertices[1] = 1.0f; m_cube_vertices[2] = 1.0f;
  m_cube_vertices[3] = 0.0f; m_cube_vertices[4] = 0.0f; m_cube_vertices[5] = 1.0f;
  m_cube_vertices[6] = 1.0f; m_cube_vertices[7] = 1.0f; m_cube_vertices[8] = 1.0f;
  m_cube_vertices[9] = 1.0f; m_cube_vertices[10] = 0.0f; m_cube_vertices[11] = 1.0f;
  m_cube_vertices[12] = 1.0f; m_cube_vertices[13] = 1.0f; m_cube_vertices[14] = 0.0f;
  m_cube_vertices[15] = 1.0f; m_cube_vertices[16] = 0.0f; m_cube_vertices[17] = 0.0f;
  m_cube_vertices[18] = 0.0f; m_cube_vertices[19] = 1.0f; m_cube_vertices[20] = 0.0f;
  m_cube_vertices[21] = 0.0f; m_cube_vertices[22] = 0.0f; m_cube_vertices[23] = 0.0f;

  m_cube_indices[0] = 0; m_cube_indices[1] = 2;
  m_cube_indices[2] = 2; m_cube_indices[3] = 3;
  m_cube_indices[4] = 3; m_cube_indices[5] = 1;
  m_cube_indices[6] = 1; m_cube_indices[7] = 0;
  m_cube_indices[8] = 6; m_cube_indices[9] = 4;
  m_cube_indices[10] = 4; m_cube_indices[11] = 5;
  m_cube_indices[12] = 5; m_cube_indices[13] = 7;
  m_cube_indices[14] = 7; m_cube_indices[15] = 6;
  m_cube_indices[16] = 0; m_cube_indices[17] = 6;
  m_cube_indices[18] = 2; m_cube_indices[19] = 4;
  m_cube_indices[20] = 1; m_cube_indices[21] = 7;
  m_cube_indices[22] = 3; m_cube_indices[23] = 5;

  update();
}

RGBHistogram::~RGBHistogram(void)
{
  if (m_points != NULL)
    delete m_points;
}

////// PRIVATE METHODS //////

void RGBHistogram::resizeGL(int width, int height)
{
  int side = qMin(width, height);
  glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void RGBHistogram::paintGL(void)
{
  glClearColor(m_bg.redF(), m_bg.greenF(), m_bg.blueF(), 1.0f);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPerspective(20.0, width() / height(), 0.01f, 1000);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glTranslatef(0.0, 0.0, -m_pos.z());
  glRotatef(m_pos.x(), 1, 0, 0);
  glRotatef(m_pos.y(), 0, 1, 0);
  glRotatef(m_rotation, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  drawCube();
  if (m_data != NULL)
    drawHistogram();
  glPopMatrix();

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void RGBHistogram::mousePressEvent(QMouseEvent* event)
{
  m_last_pos = event->pos();
}

void RGBHistogram::mouseMoveEvent(QMouseEvent* event)
{
  const QPointF diff = event->pos() - m_last_pos;

  if (event->buttons() & Qt::LeftButton)
  {
    m_pos += Point3D(diff.y(), diff.x(), 0.0f) * 0.2f;
    update();
  }
  else if (event->buttons() & Qt::RightButton)
  {
    m_rotation += diff.x() * 0.2f;
    update();
  }

  m_last_pos = event->pos();
}

void RGBHistogram::wheelEvent(QWheelEvent* event)
{
  m_pos *= Point3D(1, 1, pow(1.1f, -event->delta() / 120.0f));

  update();
}

void RGBHistogram::drawCube(void)
{
  glPushMatrix();
  glColorPointer(3, GL_FLOAT, 0, &m_cube_vertices);
  glVertexPointer(3, GL_FLOAT, 0, &m_cube_vertices);
  glTranslatef(-0.5f, -0.5f, -0.5f);
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, &m_cube_indices);
  glPopMatrix();
}

void RGBHistogram::drawHistogram(void)
{
#pragma omp parallel for
  for (int a = 0; a < m_size; a++)
  {
    int gl_pos = a * 3;
    int pos = a * 4;
    *(m_points + gl_pos) = *(m_data + pos);
    *(m_points + gl_pos + 1) = *(m_data + pos + 1);
    *(m_points + gl_pos + 2) = *(m_data + pos + 2);
  }

  glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_data);
  glVertexPointer(3, GL_FLOAT, 0, &m_points[0]);

  glPushMatrix();
  glTranslatef(-0.5f, -0.5f, -0.5f);

  // 0.003921568627451 = 1.0 / 255
  glScaled(0.003921568627451f, 0.003921568627451f, 0.003921568627451f);

  glDrawArrays(GL_POINTS, 0, m_size);
  glPopMatrix();
}

////// PRIVATE SLOTS //////

void RGBHistogram::updateCube(uint8_t* data)
{
  m_data = data;
  update();
}

void RGBHistogram::updateSize(int width, int height)
{
  m_size = width * height;
  
  if (m_points != NULL)
    delete m_points;

  m_points = (GLfloat*) malloc(m_size * 3 * sizeof(GLfloat));
}
