#ifndef COLOR_LIST_H
#define COLOR_LIST_H

#include <QAbstractListModel>
#include <QList>
#include <QColor>
#include <QBrush>
#include <QPoint>

class ColorList : public QAbstractListModel
{
  Q_OBJECT;
  
 public:
  ColorList(QObject* parent = 0);

  QList<QColor>* colorList(void);
  QList<QPoint>* coordsList(void);
  
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

  bool setData(const QModelIndex& index, const QVariant& value, const QPoint& coords, int role = Qt::EditRole);
  bool insertRow(int position, const QModelIndex& index = QModelIndex());
  bool removeRow(int position, const QModelIndex& index = QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
  
 private:
  QList<QColor> m_color_list;
  QList<QPoint> m_coords_list;
};

#endif
