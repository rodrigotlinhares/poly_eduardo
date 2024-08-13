#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QGLWidget>
#include <QResizeEvent>
#include <QPointF>
#include <QPoint>
#include <QColor>
#include <QList>
#include <cstdint>

class VideoPlayer : public QGLWidget
{
  Q_OBJECT;

 public:
  VideoPlayer(QWidget* parent = 0);

  int frameSize(void);
  
  void setList(QList<QPoint>* coords_list);
  void setPlaying(bool playing);

  bool isPlaying(void);

 signals:
  void newPoint(QColor color, QPoint coords);

 private slots:
  void updateSize(int width, int height);
  void updateFrame(uint8_t* data);

 private:
  void initializeGL(void);
  void resizeGL(float width, float height);
  void paintGL(void);
  
  void resizeEvent(QResizeEvent* event);

  void mouseMoveEvent(QMouseEvent* event);
  
 private:
  uint8_t* m_data;
  QPointF m_frame_size, m_video_size;
  QList<QPoint>* m_coords_list;
  bool m_playing;
};

#endif
