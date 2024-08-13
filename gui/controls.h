#ifndef CONTROLS_H
#define CONTROLS_H

#include <QWidget>
#include <webcamwrapper/webcamwrapper.h>

#include "ui_controls.h"

class Controls : public QWidget
{
  Q_OBJECT;

 public:
  Controls(QWidget* parent = 0);
  ~Controls(void);

 private:
  void hideEvent(QHideEvent* event);
  void checkControls(void);

 private slots:
  void updateIndex(int index);
  void newVideo(void);
  
  void turnLeft(void);
  void turnRight(void);
  void turnUp(void);
  void turnDown(void);
  void resetPosition(void);

 signals:
  void hidden(void);
  void updateExponential(int exponential);
  
 private:
  Ui::Controls* m_ui;
  WebcamWrapper* m_webcam;
  int m_index;
};

#endif
