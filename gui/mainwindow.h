#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <classifiers/pattern.h>

#include "ui_mainwindow.h"
#include "opencamera.h"
#include "camerasettings.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT;

 public:
  MainWindow(QWidget* parent = 0);
  ~MainWindow(void);

 private:
  Ui::MainWindow* m_ui;
  OpenCamera* m_open_camera;
  CameraSettings* m_camera_settings;
};

#endif
