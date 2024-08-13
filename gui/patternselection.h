#ifndef PATTERN_SELECTION_H
#define PATTERN_SELECTION_H

#include <QWidget>
#include <classifiers/pattern.h>

#include "ui_patternselection.h"
#include "colorlist.h"

class PatternSelection : public QWidget
{
  Q_OBJECT;

 public:
  PatternSelection(QWidget* parent = 0);
  ~PatternSelection(void);

  QList<QPoint>* colorList(void);

 private:
  pattern* colorPattern(void);

 private slots:
  void showEvent(QShowEvent* event);
  void hideEvent(QHideEvent* event);
  void done(void);
  void cancel(void);
  
  void addRow(QColor color, QPoint coords);
  void clearList(void);
  void clearSelected(void);
  void updateClearSelectedPB(const QItemSelection& selected, const QItemSelection& deselected);
  
 signals:
  void hidden(void);
  void freezeVideo(bool status);
  void newPattern(pattern* pattern);

 private:
  Ui::PatternSelection* m_ui;
  ColorList* m_color_list;
};

#endif
