#ifndef METHODS_H
#define METHODS_H

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QTextBlock>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QWidget>

class Methods : public QObject {
  Q_OBJECT
 public:
  explicit Methods(QObject *parent = nullptr);

  static QStringList getVoidForCpp(QString c_file);
  static QString loadText(QString textFile);
  static QString getTextEditLineText(QTextEdit *txtEdit, int i);
  static bool isSymbol(QString line);
 signals:
};

#endif  // METHODS_H
