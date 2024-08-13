#include "opencamera.h"

////// PUBLIC METHODS //////

OpenCamera::OpenCamera(QWidget* parent)
  : QDialog(parent),
    m_ui(new Ui::OpenCamera),
    m_webcam(new WebcamWrapper)
{
  m_ui->setupUi(this);

  connect(m_ui->device_cb, SIGNAL(currentIndexChanged(int)),
          this, SLOT(resolutions(int)));
  connect(m_ui->resolution_cb, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(setResolution(QString)));
}

OpenCamera::~OpenCamera(void)
{
  delete m_ui;
  delete m_webcam;
}

////// PRIVATE METHODS //////

void OpenCamera::showEvent(QShowEvent* event)
{
  Q_UNUSED(event);

  m_ui->device_cb->clear();

  delete m_webcam;
  m_webcam = new WebcamWrapper;

  for (int a = 0; a < m_webcam->size(); a++)
    m_ui->device_cb->addItem(QString(m_webcam->deviceName(a)));
}

////// PRIVATE SLOTS //////

void OpenCamera::resolutions(int index)
{
  m_ui->resolution_cb->clear();

  std::vector<uint32_t> resolutions;
  m_webcam->deviceResolutions(index, &resolutions);

  for (uint32_t a = 0; a < resolutions.size(); a += 2)
  {
    char resolution[9];
    sprintf(resolution, "%dx%d", resolutions[a], resolutions[a + 1]);
    m_ui->resolution_cb->addItem(QString(resolution));
  }
}

void OpenCamera::setResolution(QString resolution)
{
  m_resolution = resolution.toUtf8();
}

void OpenCamera::accept(void)
{
  if (m_ui->device_cb->count() != 0)
  {
    emit newCamera(m_webcam->deviceLocation(m_ui->device_cb->currentIndex()), m_resolution.data());
    emit newCameraIndex(m_ui->device_cb->currentIndex());
  }
  hide();
}
