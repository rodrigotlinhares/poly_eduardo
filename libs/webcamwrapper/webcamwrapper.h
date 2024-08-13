#ifndef WEBCAM_WRAPPER_H
#define WEBCAM_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <vector>

#include "webcam.h"

class WebcamWrapper
{
 public:
  WebcamWrapper(void);
  ~WebcamWrapper(void);

  int size(void);
  
  char* deviceLocation(int position);
  char* deviceName(int position);
  char* deviceDriver(int position);
  
  void deviceResolutions(int position, std::vector<uint32_t>* resolutions);
  void deviceControls(int position, std::vector<uint32_t>* controls);
  void addControlMapping(const char* path);

  int control(int position, _CControlId control);
  bool setControl(int position, int val, _CControlId control);
  bool checkControlSupport(int position, _CControlId control);
  int controlType(int position, _CControlId control);
  int controlMax(int position, _CControlId control);
  int controlMin(int position, _CControlId control);
  char* controlName(int position, _CControlId control);
  
 private:
  CResult m_ret;
  std::vector<CHandle> m_devices;
};

#endif
