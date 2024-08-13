#include "colorlist.h"

////// PUBLIC METHODS //////

ColorList::ColorList(QObject* parent)
  : QAbstractListModel(parent)
{
  
}

QList<QColor>* ColorList::colorList(void)
{
  return &m_color_list;
}

QList<QPoint>* ColorList::coordsList(void)
{
  return &m_coords_list;
}

int ColorList::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  
  return m_coords_list.count();
}

QVariant ColorList::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    return QVariant();

  if (index.row() >= m_coords_list.size())
    return QVariant();

  QColor color = m_color_list.at(index.row());

  switch (role)
  {
  case Qt::DisplayRole:
    return QString("R: %1, G: %2, B: %3").arg(color.red()).arg(color.green()).arg(color.blue());
  case Qt::BackgroundRole:
    return QBrush(color);
  case Qt::TextAlignmentRole:
    return Qt::AlignCenter;
  }

  return QVariant();
}

bool ColorList::setData(const QModelIndex& index, const QVariant& value, const QPoint& coords, int role)
{
  if (!index.isValid())
    return false;
    
  if (role == Qt::EditRole)
  {
    m_color_list.replace(index.row(), value.toString());
    m_coords_list.replace(index.row(), coords);
    emit dataChanged(index, index);
    return true;
  }

  return false;
}

bool ColorList::insertRow(int position, const QModelIndex& index)
{
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position + 1);

  m_color_list.insert(position, QString(""));
  m_coords_list.insert(position, QPoint());

  endInsertRows();
  return true;
}

bool ColorList::removeRow(int position, const QModelIndex& index)
{
  Q_UNUSED(index);
  
  beginRemoveRows(QModelIndex(), position, position + 1);

  m_color_list.removeAt(position);
  m_coords_list.removeAt(position);

  endRemoveRows();
  return true;
}

bool ColorList::removeRows(int position, int rows, const QModelIndex& index)
{
  Q_UNUSED(index);
  
  beginRemoveRows(QModelIndex(), position, position + rows - 1);

  for (int a = 0; a < rows; a++)
  {
    m_color_list.removeAt(position);
    m_coords_list.removeAt(position);
  }

  endRemoveRows();
  return true;
}
