#ifndef OPENCV_THREAD_H
#define OPENCV_THREAD_H

#include <cv.h>
#include <highgui.h>

#include "decoderbasis.h"

class OpenCVThread : public DecoderBasis
{
 public:
  OpenCVThread(void);
  ~OpenCVThread(void);

  void loadCamera(const char* device, const char* resolution);
  void loadVideoFile(const char* path);

  void run(void);
  void startCapture(void);
  void stopCapture(void);

  void setCurrentFrame(int frame);
  
  uint8_t* frame(void);
  int frames(void);
  int currentFrame(void);

 private:
  cv::Mat m_frame, m_tmp;
  cv::VideoCapture* m_capture;
};

#endif
