#include "opencvthread.h"

////// PUBLIC METHODS //////

OpenCVThread::OpenCVThread(void)
  : m_capture(new cv::VideoCapture)
{

}

OpenCVThread::~OpenCVThread(void)
{
  stopCapture();
  delete m_capture;
}

void OpenCVThread::loadCamera(const char* device, const char* resolution)
{
  stopCapture();

  m_capture->open(atoi(device + 5));

  char res[strlen(resolution)];
  memcpy(&res, resolution, strlen(resolution) * sizeof(char));
  
  int width = atoi(strtok(res, "x"));
  int height = atoi(strtok(NULL, "x"));

  m_capture->set(CV_CAP_PROP_FRAME_WIDTH, width);
  m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, height);
  
  m_width = m_capture->get(CV_CAP_PROP_FRAME_WIDTH);
  m_height = m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);

  // Grab first frame
  m_capture->grab();
  m_capture->retrieve(m_tmp, 0);
  cv::cvtColor(m_tmp, m_frame, CV_BGR2RGBA);

  m_frames = 1;
  m_camera = true;
}

void OpenCVThread::loadVideoFile(const char* path)
{
  stopCapture();
  
  m_capture->open(path);
  
  m_width = m_capture->get(CV_CAP_PROP_FRAME_WIDTH);
  m_height = m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);
  
  // Grab first frame
  m_capture->grab();
  m_capture->retrieve(m_tmp, 0);
  cv::cvtColor(m_tmp, m_frame, CV_BGR2RGBA);  
  
  m_frames = (uint64_t) m_capture->get(CV_CAP_PROP_FRAME_COUNT) - 1;
  m_camera = false;
}

void OpenCVThread::run(void)
{
  if (!m_capture->grab())
    return;

  m_capture->retrieve(m_tmp, 0);
  cv::cvtColor(m_tmp, m_frame, CV_BGR2RGBA);
  m_current_frame = m_camera ? 0 : (uint64_t) m_capture->get(CV_CAP_PROP_POS_FRAMES);
  m_is_new_frame_available = true;
}

void OpenCVThread::startCapture(void)
{
  // Check if the capture device has been set
  if (!m_capture->isOpened())
    return;

  start();
}

void OpenCVThread::stopCapture(void)
{
  wait();

  m_capture->release();
}

void OpenCVThread::setCurrentFrame(int frame)
{
  if((uint64_t) frame >= m_frames || frame < 0)
    return;

  m_capture->set(CV_CAP_PROP_POS_FRAMES, (double) frame);
}

uint8_t* OpenCVThread::frame(void)
{
  m_is_new_frame_available = false;
  start();
  return (uint8_t*) m_frame.data;
}

int OpenCVThread::frames(void)
{
  return m_frames;
}

int OpenCVThread::currentFrame(void)
{
  return m_current_frame;
}
