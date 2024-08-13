#include "patternselection.h"
#include <QDebug>
////// PUBLIC METHODS //////

PatternSelection::PatternSelection(QWidget* parent)
  : QWidget(parent),
    m_ui(new Ui::PatternSelection),
    m_color_list(new ColorList(this))
{
  m_ui->setupUi(this);
  hide();

  m_ui->color_list_lv->setModel(m_color_list);

  connect(m_ui->done_pb, SIGNAL(clicked()),
          this, SLOT(done()));
  connect(m_ui->cancel_pb, SIGNAL(clicked()),
          this, SLOT(cancel()));
  connect(m_color_list, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
          m_ui->color_list_lv, SLOT(scrollToBottom()));
  connect(m_ui->clear_list_pb, SIGNAL(clicked()),
          this, SLOT(clearList()));
  connect(m_ui->clear_selected_pb, SIGNAL(clicked()),
          this, SLOT(clearSelected()));
  connect(m_ui->color_list_lv->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
          this, SLOT(updateClearSelectedPB(QItemSelection, QItemSelection)));
}

PatternSelection::~PatternSelection(void)
{
  delete m_ui;
  delete m_color_list;
}

QList<QPoint>* PatternSelection::colorList(void)
{
  return m_color_list->coordsList();
}

////// PRIVATE METHODS //////

pattern* PatternSelection::colorPattern(void)
{
  int dimensions = 3;
  int size = m_color_list->colorList()->size() - 1;
  
  doubleVector* data = (doubleVector*) malloc(size * sizeof(doubleVector));
  point2D* coord = (point2D*) malloc(size * sizeof(point2D));

  QList<QColor>* color_list = m_color_list->colorList();
  QList<QPoint>* coords_list = m_color_list->coordsList();

  // TODO: Change it to use the color and coords list directly
  for (int a = 0; a < size; a++)
  {
    coord[a].x = coords_list->at(a).x();
    coord[a].y = coords_list->at(a).y();

    data[a].v = (double*) malloc(dimensions * sizeof(double));
    data[a].v[0] = color_list->at(a).red();
    data[a].v[1] = color_list->at(a).green();
    data[a].v[2] = color_list->at(a).blue();
  }

  pattern* new_pattern = new pattern(size, dimensions);
  new_pattern->setData(data);
  new_pattern->setCoordinates(coord);

  qDebug() << color_list->size();

  return new_pattern;
}

////// PRIVATE SLOTS //////

void PatternSelection::showEvent(QShowEvent* event)
{
  Q_UNUSED(event);
  emit freezeVideo(true);
}

void PatternSelection::hideEvent(QHideEvent* event)
{
  Q_UNUSED(event);
  emit freezeVideo(false);
  emit hidden();
}

void PatternSelection::done(void)
{
  emit newPattern(colorPattern());

  clearList();
  hide();
}

void PatternSelection::cancel(void)
{
  clearList();
  hide();
}

void PatternSelection::addRow(QColor color, QPoint coords)
{
  if (!isVisible())
    return;
  
  int rows = m_color_list->rowCount();

  m_color_list->insertRow(rows);
  m_color_list->setData(m_color_list->index(rows), color, coords);
}

void PatternSelection::clearList(void)
{
  int rows = m_color_list->rowCount();

  m_color_list->removeRows(0, rows);
}

void PatternSelection::clearSelected(void)
{
  QModelIndexList indexes = m_ui->color_list_lv->selectionModel()->selectedIndexes();

  for (int a = 0; a < indexes.size(); a++)
    m_color_list->removeRows(indexes[a].row(), 1);
}

void PatternSelection::updateClearSelectedPB(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected);
  
  m_ui->clear_selected_pb->setEnabled(selected.indexes().isEmpty() ? false : true);
}
