#include "camerasettings.h"

////// PUBLIC METHODS //////

CameraSettings::CameraSettings(QWidget* parent)
  : QDialog(parent),
    m_ui(new Ui::CameraSettings),
    m_webcam(new WebcamWrapper),
    m_settings(new QSettings("Lapix", "Polynom Mahalanobis")),
    m_index(0)
{
  m_ui->setupUi(this);

  connect(m_ui->brightness_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setBrightness(int)));
  connect(m_ui->contrast_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setContrast(int)));
  connect(m_ui->gain_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setGain(int)));
  connect(m_ui->saturation_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setSaturation(int)));
  connect(m_ui->hue_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setHue(int)));
  connect(m_ui->gamma_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setGamma(int)));
  connect(m_ui->sharpness_sb, SIGNAL(valueChanged(int)),
          this, SLOT(setSharpness(int)));
  connect(m_ui->expo_cb, SIGNAL(stateChanged(int)),
          this, SLOT(setExposure(int)));
  connect(m_ui->button_box, SIGNAL(accepted()),
          this, SLOT(save()));
  connect(m_ui->button_box, SIGNAL(rejected()),
          this, SLOT(restore()));
  connect(m_ui->open_file_pb, SIGNAL(clicked()),
          this, SLOT(openMapFile()));
}

CameraSettings::~CameraSettings(void)
{
  delete m_ui;
  delete m_webcam;
  delete m_settings;
}

////// PRIVATE METHODS //////

void CameraSettings::checkProperty(const char* name, _CControlId control)
{
  QWidget* widget = findChild<QWidget*>(QString(name));
  if (!widget)
    return;

  if (!strcmp(name, "map_file"))
  {
    QLineEdit* line_edit = widget->findChild<QLineEdit*>();
    if (!m_settings->contains(name))
      m_settings->setValue(name, line_edit->text());

    if (m_settings->value(name, "").toString().isEmpty())
      return;
    
    line_edit->setText(m_settings->value(name, "").toString());
    m_webcam->addControlMapping(line_edit->text().toUtf8());
    return;
  }

  if (!m_webcam->checkControlSupport(m_index, control))
  {
    widget->hide();
    return;
  }

  QSlider* slider = widget->findChild<QSlider*>();
  QSpinBox* spinbox = widget->findChild<QSpinBox*>();
  QCheckBox* checkbox = widget->findChild<QCheckBox*>();

  int max = m_webcam->controlMax(m_index, control);
  int min = m_webcam->controlMin(m_index, control);
  int value = m_settings->value(name, m_webcam->control(m_index, control)).toInt();

  if (!m_settings->contains(name))
    m_settings->setValue(name, value);

  if (spinbox)
    spinbox->setRange(min, max);
  
  if (slider)
  {
    slider->setRange(min, max);
    slider->setValue(value);
  }

  if (checkbox)
    value == 3 ? checkbox->setChecked(true) : checkbox->setChecked(false);

  m_webcam->setControl(m_index, value, control);
}


void CameraSettings::saveProperty(const char* name)
{
  QWidget* widget = findChild<QWidget*>(QString(name));
  if (!widget || !widget->isVisible())
    return;

  if (!strcmp(name, "map_file"))
  {
    QLineEdit* line_edit = widget->findChild<QLineEdit*>();
    m_settings->setValue(name, line_edit->text());
    return;
  }
  
  QSlider* slider = widget->findChild<QSlider*>();
  QCheckBox* checkbox = widget->findChild<QCheckBox*>();
  
  int value;
  if (slider)
    value = slider->value();
  if (checkbox)
    value = checkbox->isChecked() ? 3 : 1;

  m_settings->setValue(name, value);
}

void CameraSettings::showEvent(QShowEvent* event)
{
  Q_UNUSED(event);

  check();
  adjustSize();
}

////// PRIVATE SLOTS //////

void CameraSettings::check(void)
{
  checkProperty("map_file");
  checkProperty("brightness", CC_BRIGHTNESS);
  checkProperty("contrast", CC_CONTRAST);
  checkProperty("gain", CC_GAIN);
  checkProperty("saturation", CC_SATURATION);
  checkProperty("hue", CC_HUE);
  checkProperty("gamma", CC_GAMMA);
  checkProperty("sharpness", CC_SHARPNESS);
  checkProperty("auto_exposure", CC_AUTO_EXPOSURE_MODE);
}

void CameraSettings::save(void)
{
  saveProperty("map_file");
  saveProperty("brightness");
  saveProperty("contrast");
  saveProperty("gain");
  saveProperty("saturation");
  saveProperty("hue");
  saveProperty("gamma");
  saveProperty("sharpness");
  saveProperty("auto_exposure");
  hide();
}

void CameraSettings::restore(void)
{
  check();
  hide();
}

void CameraSettings::updateIndex(int index)
{
  m_index = index;
  
  if (!m_settings->group().isEmpty())
    m_settings->endGroup();

  m_settings->beginGroup(m_webcam->deviceName(m_index));

  check();
}

void CameraSettings::openMapFile(void)
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open Map File"), QDir::homePath(), tr("Map Files (*.xml)"));
  if (path.isEmpty())
    return;

  m_ui->map_file_le->setText(path);
}

void CameraSettings::setBrightness(int value)
{
  m_webcam->setControl(m_index, value, CC_BRIGHTNESS);
}

void CameraSettings::setContrast(int value)
{
  m_webcam->setControl(m_index, value, CC_CONTRAST);
}

void CameraSettings::setGain(int value)
{
  m_webcam->setControl(m_index, value, CC_GAIN);
}

void CameraSettings::setSaturation(int value)
{
  m_webcam->setControl(m_index, value, CC_SATURATION);
}

void CameraSettings::setHue(int value)
{
  m_webcam->setControl(m_index, value, CC_HUE);
}

void CameraSettings::setGamma(int value)
{
  m_webcam->setControl(m_index, value, CC_GAMMA);
}

void CameraSettings::setSharpness(int value)
{
  m_webcam->setControl(m_index, value, CC_SHARPNESS);
}

void CameraSettings::setExposure(int value)
{
  if (value)
    m_webcam->setControl(m_index, 3, CC_AUTO_EXPOSURE_MODE);
  else
    m_webcam->setControl(m_index, 1, CC_AUTO_EXPOSURE_MODE);
}
