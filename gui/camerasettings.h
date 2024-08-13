#ifndef CAMERA_SETTINGS_H
#define CAMERA_SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <webcamwrapper/webcamwrapper.h>

#include "ui_camerasettings.h"

class CameraSettings : public QDialog
{
  Q_OBJECT;

 public:
  CameraSettings(QWidget* parent = 0);
  ~CameraSettings(void);

 private:
  void checkProperty(const char* name, _CControlId control = CC_V4L2_BASE);
  void saveProperty(const char* name);
  void showEvent(QShowEvent* event);

 private slots:
  void check(void);
  void save(void);
  void restore(void);
  void updateIndex(int index);
  void openMapFile(void);
  
  void setBrightness(int value);
  void setContrast(int value);
  void setGain(int value);
  void setSaturation(int value);
  void setHue(int value);
  void setGamma(int value);
  void setSharpness(int value);
  void setExposure(int value);

 private:
  Ui::CameraSettings* m_ui;
  WebcamWrapper* m_webcam;
  QSettings* m_settings;
  int m_index;
};

#endif
