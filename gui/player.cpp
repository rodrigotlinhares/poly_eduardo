#include "player.h"

#include <QDebug>

////// PUBLIC METHODS //////

Player::Player(QWidget* parent)
  : QWidget(parent),
    m_ui(new Ui::Player),
    m_timer(new QTimer),
    m_decoder(new OpenCVThread),
    m_classifier(new classifiers::polyMahalanobis),
    m_threshold(new FILTERS::Threshold),
    m_convert(new FILTERS::Conversion),
    m_fps(33),
    m_loaded(false),
    m_output(NULL),
    m_space(false),
    m_exponential(-1)
{
  m_ui->setupUi(this);

  m_ui->player_controls->hide();

  connect(m_timer, SIGNAL(timeout()),
          this, SLOT(nextFrame()));

  // Set controls connections
  connect(this, SIGNAL(playing(bool)),
          this, SLOT(updatePlayingStatus(bool)));
  connect(m_ui->toggle_play_pb, SIGNAL(clicked()),
          this, SLOT(togglePlay()));
  connect(m_ui->stop_pb, SIGNAL(clicked()),
          this, SLOT(stop()));

  // Set slider connections
  connect(m_ui->slider_sld, SIGNAL(sliderPressed()),
          this, SLOT(pause()));
  connect(m_ui->slider_sld, SIGNAL(sliderMoved(int)),
          this, SLOT(setVideoFrame(int)));
  connect(m_ui->slider_sld, SIGNAL(sliderReleased()),
          this, SLOT(play()));

  // Set video player connections
  connect(this, SIGNAL(newSize(int, int)),
          m_ui->video_player, SLOT(updateSize(int, int)));
  connect(this, SIGNAL(newFrame(uint8_t*)),
          m_ui->video_player, SLOT(updateFrame(uint8_t*)));
}

Player::~Player(void)
{
  delete m_ui;
  delete m_timer;
  delete m_decoder;
  delete m_classifier;
}

////// PRIVATE METHODS //////

void Player::resetSimilarity(void)
{
#pragma omp parallel for
  for (int a = 0; a < 16777216; a++)
    m_similarity[a] = -1;
}

uint8_t* Player::evaluateToMap(uint8_t* data, uint32_t size, uint32_t dimensions, uint32_t order)
{
  if (m_output != NULL)
    free(m_output);
  m_output = (uint8_t*) malloc(size * 4 * sizeof(uint8_t));
  
#pragma omp parallel for
  for(uint32_t a = 0; a < size; a++)
  {
    int d_pos = a * 4;
    int pos = data[d_pos] * 256 * 256 + data[d_pos + 1] * 256 + data[d_pos + 2];
    double value = m_similarity[pos];
    if (value == -1)
    {
      double v[3];
      v[0] = data[d_pos];
      v[1] = data[d_pos + 1];
      v[2] = data[d_pos + 2];
      
      double* intensity = m_classifier->evaluateToCenter(v, 1, dimensions);
      value = intensity[order - 1];
      m_similarity[pos] = value;

      free(intensity);
    }

    double sim = exp(m_exponential * value);

    m_output[d_pos] = sim * 255;
    m_output[d_pos + 1] = sim * 255;
    m_output[d_pos + 2] = sim * 255;
    m_output[d_pos + 3] = 255;
  }

  return m_output;
}

void Player::fillSimilarity(void)
{
#pragma omp parallel for
  for (int a = 0; a < 16777216; a++)
  {
    int x, y, z;
    x = a / 256;
    y = (a - x * 256) / 256;
    z = (a - x * 256) - y * 256;

    double v[3] = {(double) x, (double) y, (double) z};
    double* intensity = m_classifier->evaluateToCenter((double*) v, 1, 3);
    m_similarity[a] = intensity[2];
    free(intensity);
  }
}

////// PRIVATE SLOTS //////

void Player::openVideo(void)
{
  QByteArray path = QFileDialog::getOpenFileName(this, tr("Open Video File"), QDir::homePath(), tr("Videos (*.avi *.asf *.mp4 *.mpg *.mov *.mkv *.ogv)")).toUtf8();
  if (path.isEmpty())
    return;

  m_decoder->loadVideoFile(path.data());
  m_decoder->startCapture();

  emit newSize(m_decoder->width(), m_decoder->height());
  m_threshold->setFrameSize(m_decoder->width(), m_decoder->height());
  m_convert->setFrameSize(m_decoder->width(), m_decoder->height());

  m_threshold->initialize();
  m_convert->initialize();
  
  m_ui->slider_sld->setRange(0, m_decoder->frames());
  m_ui->slider_sld->setValue(0);
  emit newVideo();

  m_ui->player_controls->show();

  m_loaded = true;
  play();
}

void Player::openCamera(const char* camera, const char* resolution)
{
  m_decoder->loadCamera(camera, resolution);
  m_decoder->startCapture();

  emit newSize(m_decoder->width(), m_decoder->height());

  m_threshold->setFrameSize(m_decoder->width(), m_decoder->height());
  m_convert->setFrameSize(m_decoder->width(), m_decoder->height());

  m_threshold->initialize();
  m_convert->initialize();

  m_ui->player_controls->hide();

  m_loaded = true;
  play();
}

void Player::nextFrame(void)
{
  if (m_decoder->currentFrame() == m_decoder->frames())
    stop();
  
  if (!m_decoder->isNewFrameAvailable())
    return;

  if(!m_ui->video_player->isPlaying())
    m_ui->video_player->updateGL();
  else
  {
    m_ui->slider_sld->setValue(m_decoder->currentFrame());
    
    if (m_space)
    {
      m_threshold->threshold(evaluateToMap(m_decoder->frame(), m_ui->video_player->frameSize(), 3, 3), 130);
      m_convert->convert(m_threshold->data(), FILTERS::Conversion::Bin2Gray);
      emit newFrame(m_convert->charData());
    }
    else
      emit newFrame(m_decoder->frame());
  }
}

void Player::setVideoFrame(int frame)
{
  m_decoder->setCurrentFrame(frame);
  emit newFrame(m_decoder->frame());
}

void Player::togglePlay(void)
{
  if (!m_loaded)
    return;

  m_ui->video_player->isPlaying() ? pause() : play();
}

void Player::play(void)
{
  emit playing(true);
}

void Player::pause(void)
{
  emit playing(false);
}

void Player::stop(void)
{
  if (!m_loaded)
    return;
  
  setVideoFrame(0);
  m_ui->slider_sld->setValue(0);
  pause();
}

void Player::freeze(bool status)
{
  m_ui->video_player->setPlaying(!status);
}

void Player::updatePlayingStatus(bool status)
{
  m_ui->video_player->setPlaying(status);

  status ? m_timer->start(m_fps) : m_timer->stop();

  QIcon icon;
  icon.addFile(status ? ":/icons/pause.png" : ":/icons/play.png");
  m_ui->toggle_play_pb->setIcon(icon);
}

void Player::updateExponential(int exponential)
{
  m_exponential = (double) exponential;
}

void Player::makeSpace(pattern* pattern)
{
  m_classifier->setPattern(pattern);
  m_classifier->makeSpace(3);

  resetSimilarity();
  // fillSimilarity();
  m_space = true;
}
