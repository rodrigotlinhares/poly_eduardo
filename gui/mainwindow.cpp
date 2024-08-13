#include "mainwindow.h"

////// PUBLIC METHODS //////

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_open_camera(new OpenCamera(this)),
    m_camera_settings(new CameraSettings(this))
{
  m_ui->setupUi(this);

  VideoPlayer* video_player_widget = m_ui->player->findChild<VideoPlayer*>();
  video_player_widget->setList(m_ui->pattern_selection->colorList());
  
  connect(video_player_widget, SIGNAL(newPoint(QColor, QPoint)),
          m_ui->pattern_selection, SLOT(addRow(QColor, QPoint)));

  connect(m_ui->open_video_act, SIGNAL(triggered()),
          m_ui->player, SLOT(openVideo()));
    connect(m_ui->open_camera_act, SIGNAL(triggered()),
          m_open_camera, SLOT(show()));
  connect(m_ui->camera_settings_act, SIGNAL(triggered()),
          m_camera_settings, SLOT(show()));

  connect(m_ui->player, SIGNAL(newFrame(uint8_t*)),
          m_ui->rgb_cube, SLOT(updateCube(uint8_t*)));
  connect(m_ui->player, SIGNAL(newSize(int, int)),
          m_ui->rgb_cube, SLOT(updateSize(int, int)));
  connect(m_ui->pattern_selection, SIGNAL(freezeVideo(bool)),
          m_ui->player, SLOT(freeze(bool)));
  connect(m_ui->pattern_selection, SIGNAL(newPattern(pattern*)),
          m_ui->player, SLOT(makeSpace(pattern*)));
  connect(m_ui->controls, SIGNAL(updateExponential(int)),
          m_ui->player, SLOT(updateExponential(int)));
  connect(m_open_camera, SIGNAL(newCamera(const char*, const char*)),
          m_ui->player, SLOT(openCamera(const char*, const char*)));
  connect(m_open_camera, SIGNAL(newCameraIndex(int)),
          m_ui->controls, SLOT(updateIndex(int)));
  connect(m_ui->player, SIGNAL(newVideo()),
          m_ui->controls, SLOT(newVideo()));
  connect(m_open_camera, SIGNAL(newCameraIndex(int)),
          m_camera_settings, SLOT(updateIndex(int)));

  connect(m_ui->controls, SIGNAL(hidden()),
          m_ui->pattern_selection, SLOT(show()));
  connect(m_ui->pattern_selection, SIGNAL(hidden()),
          m_ui->controls, SLOT(show()));
}

MainWindow::~MainWindow(void)
{
  delete m_ui;
  delete m_open_camera;
  delete m_camera_settings;
}
