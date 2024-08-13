#ifndef DECODER_BASIS_H
#define DECODER_BASIS_H

#include <QThread>
#include <stdint.h>

class DecoderBasis : public QThread
{
 public:
  DecoderBasis(void);
  
  virtual void loadCamera(const char* device, const char* resolution) = 0;
  virtual void loadVideoFile(const char* path) = 0;

  virtual void run(void) = 0;
  virtual void startCapture(void) = 0;
  virtual void stopCapture(void) = 0;

  virtual void setCurrentFrame(int frame) = 0;
  
  int width(void);
  int height(void);
  bool isNewFrameAvailable(void);
  virtual uint8_t* frame(void) = 0;
  virtual int frames(void) = 0;
  virtual int currentFrame(void) = 0;
  
 protected:
  bool m_is_new_frame_available, m_camera;
  int m_width, m_height;
  uint64_t m_frames, m_current_frame;
};

#endif
