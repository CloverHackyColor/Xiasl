#include "methods.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

extern QList<QTreeWidgetItem*> tw_list;
extern MainWindow* mw_one;

Methods::Methods(QObject* parent) : QObject{parent} {}

void Methods::setColorMatch(int red, QsciLexer* textLexer) {
  if (red < 55)  //暗模式，mac下为50
  {
    textLexer->setColor(QColor(30, 190, 30),
                        QsciLexerCPP::CommentLine);  //"//"注释颜色
    textLexer->setColor(QColor(30, 190, 30), QsciLexerCPP::Comment);

    textLexer->setColor(QColor(210, 210, 210), QsciLexerCPP::Identifier);
    textLexer->setColor(QColor(245, 150, 147), QsciLexerCPP::Number);
    textLexer->setColor(QColor(100, 100, 250), QsciLexerCPP::Keyword);
    textLexer->setColor(QColor(210, 32, 240), QsciLexerCPP::KeywordSet2);
    textLexer->setColor(QColor(245, 245, 245), QsciLexerCPP::Operator);
    textLexer->setColor(QColor(84, 235, 159),
                        QsciLexerCPP::DoubleQuotedString);  //双引号
  } else {
    textLexer->setColor(QColor(30, 190, 30),
                        QsciLexerCPP::CommentLine);  //"//"注释颜色
    textLexer->setColor(QColor(30, 190, 30), QsciLexerCPP::Comment);

    textLexer->setColor(QColor(13, 136, 91), QsciLexerCPP::Number);
    textLexer->setColor(QColor(0, 0, 255), QsciLexerCPP::Keyword);
    textLexer->setColor(QColor(0, 0, 0), QsciLexerCPP::Identifier);
    textLexer->setColor(QColor(210, 0, 210), QsciLexerCPP::KeywordSet2);
    textLexer->setColor(QColor(20, 20, 20), QsciLexerCPP::Operator);
    textLexer->setColor(QColor(163, 21, 21),
                        QsciLexerCPP::DoubleQuotedString);  //双引号
  }
}

QStringList Methods::getVoidForCpp(QsciScintilla* textEdit) {
  QStringList listVoid;

  QTextEdit* txtEdit = new QTextEdit;
  txtEdit->setPlainText(textEdit->text());

  for (int i = 0; i < txtEdit->document()->lineCount(); i++) {
    QString line = getTextEditLineText(txtEdit, i).trimmed();
    if (isSymbol(line)) {
      if (line.mid(0, 1) != "/") {
        QStringList list1 = line.split("(");
        if (list1.count() > 1) {
          QString str1 = list1.at(0) + "|" + QString::number(i);
          listVoid.append(str1.trimmed());
        }
      }
    }
  }

  tw_list.clear();
  for (int i = 0; i < listVoid.count(); i++) {
    QString str0 = listVoid.at(i);
    QStringList list0 = str0.split("|");
    QTreeWidgetItem* twItem0 =
        new QTreeWidgetItem(QStringList() << list0.at(0) << list0.at(1));
    twItem0->setIcon(0, QIcon(":/icon/m.svg"));
    tw_list.append(twItem0);
  }

  return listVoid;
}

bool Methods::isSymbol(QString line) {
  line.trimmed();
  QStringList listKeys = QStringList() << "static"
                                       << "void"
                                       << "int"
                                       << "bool"
                                       << "const"
                                       << "virtual"
                                       << "class"
                                       << "long"
                                       << "signed"
                                       << "unsigned"
                                       << "typedef"
                                       << "friend"
                                       << "template"
                                       << "enum"
                                       << "inline"
                                       << "short"
                                       << "struct"
                                       << "union"
                                       << "double"
                                       << "float"
                                       << "string"
                                       << "char"
                                       << "wchar_t"
                                       << "QString"
                                       << "StringList";
  for (int i = 0; i < listKeys.count(); i++) {
    QString str0 = listKeys.at(i);
    if (line.mid(0, str0.length()) == str0 && !line.contains("=") &&
        !line.contains("<<") && !line.contains("+") && !line.contains("%") &&
        !line.contains("//") && !line.contains("number"))
      return true;
  }
  return false;
}

QString Methods::loadText(QString textFile) {
  QFileInfo fi(textFile);
  if (fi.exists()) {
    QFile file(textFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(
          NULL, tr("Application"),
          tr("Cannot read file %1:\n%2.")
              .arg(QDir::toNativeSeparators(textFile), file.errorString()));

    } else {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      QString text = in.readAll();
      return text;
    }
  }

  return "";
}

QString Methods::getTextEditLineText(QTextEdit* txtEdit, int i) {
  QTextBlock block = txtEdit->document()->findBlockByNumber(i);
  txtEdit->setTextCursor(QTextCursor(block));
  QString lineText = txtEdit->document()->findBlockByNumber(i).text().trimmed();
  return lineText;
}
