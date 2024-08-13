#include "controls.h"
#include <QDebug>
////// PUBLIC METHODS //////

Controls::Controls(QWidget* parent)
  : QWidget(parent),
    m_ui(new Ui::Controls),
    m_index(0)
{
  m_ui->setupUi(this);

  m_ui->webcam_controls->setEnabled(false);
  m_ui->video_controls->setEnabled(false);

  m_webcam = new WebcamWrapper;

  connect(m_ui->left_pb, SIGNAL(clicked()),
          this, SLOT(turnLeft()));
  connect(m_ui->right_pb, SIGNAL(clicked()),
          this, SLOT(turnRight()));
  connect(m_ui->up_pb, SIGNAL(clicked()),
          this, SLOT(turnUp()));
  connect(m_ui->down_pb, SIGNAL(clicked()),
          this, SLOT(turnDown()));
  connect(m_ui->reset_position_pb, SIGNAL(clicked()),
          this, SLOT(resetPosition()));
  connect(m_ui->pattern_selection_pb, SIGNAL(clicked()),
          this, SLOT(hide()));
  connect(m_ui->exp_value_sb, SIGNAL(valueChanged(int)),
          this, SIGNAL(updateExponential(int)));
}

Controls::~Controls(void)
{
  delete m_ui;
  delete m_webcam;
}

////// PRIVATE METHODS //////

void Controls::hideEvent(QHideEvent* event)
{
  Q_UNUSED(event);

  emit hidden();
}

void Controls::checkControls(void)
{
  m_ui->webcam_controls->setEnabled(false);
  if (!m_webcam->checkControlSupport(m_index, CC_PAN_RELATIVE))
    return;
  if (!m_webcam->checkControlSupport(m_index, CC_TILT_RELATIVE))
    return;
  if (!m_webcam->checkControlSupport(m_index, CC_PAN_RESET) &&
      !m_webcam->checkControlSupport(m_index, CC_TILT_RESET))
    return;

  m_ui->webcam_controls->setEnabled(true);
}

////// PRIVATE SLOTS //////

void Controls::updateIndex(int index)
{
  m_index = index;

  m_ui->webcam_controls->setEnabled(true);
  m_ui->video_controls->setEnabled(true);
  checkControls();
}

void Controls::newVideo(void)
{
  qDebug() << "teste";

  m_ui->webcam_controls->setEnabled(false);
  m_ui->video_controls->setEnabled(true);
}

void Controls::turnLeft(void)
{
  m_webcam->setControl(m_index, -500, CC_PAN_RELATIVE);
}

void Controls::turnRight(void)
{
  m_webcam->setControl(m_index, 500, CC_PAN_RELATIVE);
}

void Controls::turnUp(void)
{
  m_webcam->setControl(m_index, -500, CC_TILT_RELATIVE);
}

void Controls::turnDown(void)
{
  m_webcam->setControl(m_index, 500, CC_TILT_RELATIVE);
}

void Controls::resetPosition(void)
{
  m_webcam->setControl(m_index, 1, CC_PAN_RESET);
  m_webcam->setControl(m_index, 1, CC_TILT_RESET);
}
