#ifndef OPEN_CAMERA_H
#define OPEN_CAMERA_H

#include <QDialog>
#include <QFileDialog>
#include <cstdint>
#include <webcamwrapper/webcamwrapper.h>

#include "ui_opencamera.h"

class OpenCamera : public QDialog
{
  Q_OBJECT;

 public:
  OpenCamera(QWidget* parent = 0);
  ~OpenCamera(void);

 private:
  void showEvent(QShowEvent* event);

 signals:
  void newCamera(const char* camera, const char* resolution);
  void newCameraIndex(int index);

 private slots:
  void resolutions(int index);
  void setResolution(QString resolution);
  void accept(void);

 private:
  Ui::OpenCamera* m_ui;
  WebcamWrapper* m_webcam;
  QByteArray m_resolution;
};

#endif
