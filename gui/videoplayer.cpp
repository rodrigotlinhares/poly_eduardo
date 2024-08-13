#include "videoplayer.h"
#include <QDebug>
////// PUBLIC METHODS //////

VideoPlayer::VideoPlayer(QWidget* parent)
  : QGLWidget(parent),
    m_data(NULL),
    m_frame_size(QPointF()),
    m_video_size(QPointF()),
    m_coords_list(NULL),
    m_playing(false)
{
  
}

int VideoPlayer::frameSize(void)
{
  return m_frame_size.x() * m_frame_size.y();
}

void VideoPlayer::setList(QList<QPoint>* coords_list)
{
  m_coords_list = coords_list;
}

void VideoPlayer::setPlaying(bool playing)
{
  m_playing = playing;
}

bool VideoPlayer::isPlaying(void)
{
  return m_playing;
}

////// PRIVATE SLOTS //////

void VideoPlayer::updateSize(int width, int height)
{
  m_frame_size.setX((float) width);
  m_frame_size.setY((float) height);

  resizeGL((float) this->width(), (float) this->height());
}

void VideoPlayer::updateFrame(uint8_t* data)
{
  m_data = data;
  glDraw();
}

////// PRIVATE METHODS //////

void VideoPlayer::initializeGL(void)
{
  glBindTexture(GL_TEXTURE_2D, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void VideoPlayer::resizeGL(float width, float height)
{
  float frame_scale = m_frame_size.x() / m_frame_size.y();
  float widget_scale = width / height;

  if (widget_scale < frame_scale)
    height = (width * m_frame_size.y()) / m_frame_size.x();
  if (widget_scale > frame_scale)
    width = (height * m_frame_size.x()) / m_frame_size.y();

  m_video_size.setX(width);
  m_video_size.setY(height);
}

void VideoPlayer::paintGL(void)
{
  QPainter painter(this);

  painter.beginNativePainting();

  if (m_data != NULL)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_frame_size.x(), m_frame_size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();

  glTranslatef((width() - m_video_size.x()) / 2, (height() - m_video_size.y()) / 2, 0.0f);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(m_video_size.x(), 0.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(m_video_size.x(), m_video_size.y());
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.0f, m_video_size.y());
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
  
  painter.endNativePainting();
  
  int bb_x = (width() - m_video_size.x()) * 0.5;
  int bb_y = (height() - m_video_size.y()) * 0.5;

  painter.setOpacity(0.5f);
  for (int a = 0; a < m_coords_list->size(); a++)
  {
    int x = (m_coords_list->at(a).x() * m_video_size.x()) / m_frame_size.x() + bb_x;
    int y = (m_coords_list->at(a).y() * m_video_size.y()) / m_frame_size.y() + bb_y;
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(x - 5, y - 5, 10, 10);
  }
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
  resizeGL((float) event->size().width(), (float) event->size().height());
}

void VideoPlayer::mouseMoveEvent(QMouseEvent* event)
{
  if (m_playing)
    return;
  
  int x = event->x();
  int y = event->y();

  int bb_x = (width() - m_video_size.x()) * 0.5;
  if (x < bb_x || x >= bb_x + m_video_size.x())
    return;
  x = (x - bb_x) * m_frame_size.x() / m_video_size.x();

  int bb_y = (height() - m_video_size.y()) * 0.5;
  if (y < bb_y || y >= bb_y + m_video_size.y())
    return;
  y = (y - bb_y) * m_frame_size.y() / m_video_size.y();

  int pos = y * m_frame_size.x() * 4 + x * 4;

  emit newPoint(QColor(m_data[pos], m_data[pos + 1], m_data[pos + 2]), QPoint(x, y));
}
