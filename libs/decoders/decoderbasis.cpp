#include "decoderbasis.h"

DecoderBasis::DecoderBasis(void)
 : m_is_new_frame_available(false),
   m_camera(false),
   m_width(0),
   m_height(0),
   m_frames(0),
   m_current_frame(0)
{
  
}

int DecoderBasis::width(void)
{
  return m_width;
}

int DecoderBasis::height(void)
{
  return m_height;
}

bool DecoderBasis::isNewFrameAvailable(void)
{
  return m_is_new_frame_available;
}
