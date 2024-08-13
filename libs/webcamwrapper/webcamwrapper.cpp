#include "webcamwrapper.h"

////// PUBLIC METHODS //////

WebcamWrapper::WebcamWrapper(void)
{
  c_cleanup();
  c_init();
  
  uint32_t size, count;
  size = count = 0;
  CDevice* devices = NULL;

  m_ret = c_enum_devices(devices, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    devices = (CDevice*) malloc(size);
    m_ret = c_enum_devices(devices, &size, &count);
    assert(m_ret == C_SUCCESS);

    // Add every found device to vector
    for (uint32_t a = 0; a < count; a++)
      m_devices.push_back(c_open_device(devices[a].shortName));

    free(devices);
  }
}

WebcamWrapper::~WebcamWrapper(void)
{
  for (uint32_t a = 0; a < m_devices.size(); a++)
    c_close_device(m_devices[a]);
  
  c_cleanup();
}

int WebcamWrapper::size(void)
{
  return m_devices.size();
}

char* WebcamWrapper::deviceLocation(int position)
{
  uint32_t size;
  size = 0;
  CDevice* info = NULL;
  char* location;

  m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    info = (CDevice*) malloc(size);
    m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
    assert(m_ret == C_SUCCESS);

    location = info->shortName;
    free(info);
    return location;
  }
  return NULL;
}

char* WebcamWrapper::deviceName(int position)
{
  uint32_t size = 0;
  CDevice* info = NULL;
  char* name;

  m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    info = (CDevice*) malloc(size);
    m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
    assert(m_ret == C_SUCCESS);

    name = info->name;
    free(info);
    return name;
  }
  return NULL;
}

char* WebcamWrapper::deviceDriver(int position)
{
  uint32_t size = 0;
  CDevice* info = NULL;
  char* driver;

  m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    info = (CDevice*) malloc(size);
    m_ret = c_get_device_info(m_devices[position], NULL, info, &size);
    assert(m_ret == C_SUCCESS);

    driver = info->driver;
    free(info);
    return driver;
  }
  return NULL;
}

void WebcamWrapper::deviceResolutions(int position, std::vector<uint32_t>* resolutions)
{
  uint32_t size, count;
  size = count = 0;
  CPixelFormat* pixel_format = NULL;
  CFrameSize* sizes = NULL;
  
  m_ret = c_enum_pixel_formats(m_devices[position], pixel_format, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    pixel_format = (CPixelFormat*) malloc(size);
    m_ret = c_enum_pixel_formats(m_devices[position], pixel_format, &size, &count);
    assert(m_ret == C_SUCCESS);

    m_ret = c_enum_frame_sizes(m_devices[position], pixel_format, sizes, &size, &count);
    if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
    {
      sizes = (CFrameSize*) malloc(size);
      m_ret = c_enum_frame_sizes(m_devices[position], pixel_format, sizes, &size, &count);
      assert(m_ret == C_SUCCESS);

      for (unsigned int a = 0; a < count; a++)
        if (sizes[a].type == CF_SIZE_DISCRETE)
        {
          resolutions->push_back(sizes[a].width);
          resolutions->push_back(sizes[a].height);
        }
        else if (sizes[a].type == CF_SIZE_CONTINUOUS)
        {
          printf("Continuous:\n\tMin:\n\t\tWidth: %u\n\t\tHeight: %u\n\tMax:\n\t\tWidth: %u\n\t\tHeight: %u\n", sizes[a].min_width, sizes[a].min_height, sizes[a].max_width, sizes[a].max_height);
          break;
        }
        else if (sizes[a].type == CF_SIZE_STEPWISE)
        {
          printf("Stepwise:\n\tMin:\n\t\tWidth: %u\n\t\tHeight: %u\n\tMax:\n\t\tWidth: %u\n\t\tHeight: %u\n\tStepsize:\n\t\tWidth: %u\n\t\tHeight: %u\n", sizes[a].min_width, sizes[a].min_height, sizes[a].max_width, sizes[a].max_height, sizes[a].step_width, sizes[a].step_height);
          break;
        }
      
      free (sizes);
    }
    free(pixel_format);
  }
}

void WebcamWrapper::deviceControls(int position, std::vector<uint32_t>* controls)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      controls->push_back(ctrls[a].id);

    free(ctrls);
  }
}

void WebcamWrapper::addControlMapping(const char* path)
{
  CDynctrlInfo* info = NULL;

  m_ret = c_add_control_mappings_from_file(path, info);
  assert(m_ret == C_SUCCESS);

  free(info);
}

int WebcamWrapper::control(int position, _CControlId control)
{
  if (checkControlSupport(position, control))
  {
    CControlValue value;
    m_ret = c_get_control(m_devices[position], control, &value);
    assert(m_ret == C_SUCCESS);
    
    return value.value;
  }
  return NULL;
}

bool WebcamWrapper::setControl(int position, int val, _CControlId control)
{
  if (checkControlSupport(position, control))
  {
    if (controlType(position, control) == CC_TYPE_DWORD)
    {
      int min = controlMin(position, control);
      if (val < min)
        val = min;
      
      int max = controlMax(position, control);
      if (val > max)
        val = max;
    }
    
    CControlValue value;
    value.value = val;
    m_ret = c_set_control(m_devices[position], control, &value);
    assert(m_ret == C_SUCCESS);

    return true;
  }
  return false;
}

bool WebcamWrapper::checkControlSupport(int position, _CControlId control)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;
  bool ret = false;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      if (ctrls[a].id == control)
        ret = true;

    free(ctrls);
  }
  return ret;
}

int WebcamWrapper::controlType(int position, _CControlId control)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;
  int type = NULL;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      if (ctrls[a].id == control)
        type = ctrls[a].type;

    free(ctrls);
  }
  return type;
}

int WebcamWrapper::controlMax(int position, _CControlId control)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;
  int max = NULL;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      if (ctrls[a].id == control)
        max = ctrls[a].max.value;

    free(ctrls);
  }
  return max;
}

int WebcamWrapper::controlMin(int position, _CControlId control)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;
  int min = NULL;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      if (ctrls[a].id == control)
        min = ctrls[a].min.value;

    free(ctrls);
  }
  return min;
}

char* WebcamWrapper::controlName(int position, _CControlId control)
{
  uint32_t size, count;
  size = count = 0;
  CControl* ctrls = NULL;
  char* name = NULL;

  m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
  if (m_ret == C_BUFFER_TOO_SMALL || m_ret == C_SUCCESS)
  {
    ctrls = (CControl*) malloc(size);
    m_ret = c_enum_controls(m_devices[position], ctrls, &size, &count);
    assert(m_ret == C_SUCCESS);

    for (uint32_t a = 0; a < count; a++)
      if (ctrls[a].id == control)
      {
        name = (char*) calloc(strlen(ctrls[a].name), sizeof(char));
        name = ctrls[a].name;
      }

    free(ctrls);
  }
  return name;
}
