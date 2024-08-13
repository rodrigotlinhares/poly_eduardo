#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QTimer>
#include <QFileDialog>
#include <decoders/opencvthread.h>
#include <classifiers/pattern.h>
#include <classifiers/polymahalanobis.h>
#include <filters/threshold.h>
#include <filters/conversion.h>
#include <omp.h>

#include "ui_player.h"

class Player : public QWidget
{
  Q_OBJECT;

 public:
  Player(QWidget* parent = 0);
  ~Player(void);

 signals:
  void playing(bool state);
  void newFrame(uint8_t* data);
  void newSize(int width, int height);
  void newCameraIndex(int index);
  void newVideo(void);

 private:
  void resetSimilarity(void);
  uint8_t* evaluateToMap(uint8_t* data, uint32_t size, uint32_t dimensions, uint32_t order);
  void fillSimilarity(void);

 private slots:
  void openVideo(void);
  void openCamera(const char* camera, const char* resolution);
  void nextFrame(void);
  void setVideoFrame(int frame);

  void togglePlay(void);
  void play(void);
  void pause(void);
  void stop(void);
  void freeze(bool status);
  void updatePlayingStatus(bool status);
  void updateExponential(int exponential);

  void makeSpace(pattern* pattern);

 private:
  Ui::Player* m_ui;
  QTimer* m_timer;
  DecoderBasis* m_decoder;
  classifiers::polyMahalanobis* m_classifier;
  FILTERS::Threshold* m_threshold;
  FILTERS::Conversion* m_convert;
  double m_similarity[16777216];
  int m_fps;
  bool m_loaded;

  uint8_t* m_output;
  bool m_space;
  double m_exponential;
};

#endif
