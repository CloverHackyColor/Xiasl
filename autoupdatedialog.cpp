#include "autoupdatedialog.h"

#include "mainwindow.h"
#include "ui_autoupdatedialog.h"

extern MainWindow* mw_one;

AutoUpdateDialog::AutoUpdateDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::AutoUpdateDialog) {
  ui->setupUi(this);

  QFileInfo fi(qAppName());
  strLinuxTargetFile = fi.absoluteFilePath();

  setWindowTitle("");
  ui->progressBar->setTextVisible(false);
  Init();
  tempDir = QDir::homePath() + "/tempqtiasl/";
  mw_one->deleteDirfile(tempDir);
  ui->label->setVisible(false);
}

AutoUpdateDialog::~AutoUpdateDialog() { delete ui; }

void AutoUpdateDialog::Init() {
  //"https://raw.fastgit.org/ic005k/"  //HK
  strWinUrl =
      "https://ghproxy.com/https://raw.githubusercontent.com/ic005k/"
      "QtOpenCoreConfigDatabase/main/win.zip";

  strMacUrl =
      "https://ghproxy.com/https://raw.githubusercontent.com/ic005k/"
      "QtOpenCoreConfigDatabase/main/Contents.zip";

  strMacClassicalUrl =
      "https://ghproxy.com/https://github.com/ic005k/QtOpenCoreConfigDatabase/"
      "releases/download/1.0.0/Contents.zip";

  strLinuxUrl =
      "https://ghproxy.com/https://github.com/ic005k/QtOpenCoreConfigDatabase/"
      "releases/download/1.0.0/OCAuxiliaryTools-Linux-x86_64.AppImage";

  strDatabaseUrl =
      "https://ghproxy.com/https://raw.githubusercontent.com/ic005k/"
      "QtOpenCoreConfigDatabase/main/Database.zip";

  manager = new QNetworkAccessManager(this);
  myfile = new QFile(this);
}

void AutoUpdateDialog::doProcessReadyRead()  //读取并写入
{
  while (!reply->atEnd()) {
    QByteArray ba = reply->readAll();
    myfile->write(ba);
  }
}

void AutoUpdateDialog::doProcessFinished() { myfile->close(); }

void AutoUpdateDialog::doProcessDownloadProgress(qint64 recv_total,
                                                 qint64 all_total)  //显示
{
  ui->progressBar->setMaximum(all_total);
  ui->progressBar->setValue(recv_total);
  setWindowTitle(tr("Download Progress") + " : " + GetFileSize(recv_total) +
                 " -> " + GetFileSize(all_total));

  if (recv_total == all_total) {
    if (recv_total < 10000) {
      return;
    }
    ui->btnStartUpdate->setEnabled(true);

    this->repaint();
    if ((mw_one->win) && ui->btnStartUpdate->isVisible()) {
      ui->label->setVisible(false);
    }

    if (mw_one->linuxOS) {
      QProcess* p = new QProcess;
      p->start("chmod", QStringList() << "+x" << tempDir + filename);
    }
  }
}

void AutoUpdateDialog::doProcessError(QNetworkReply::NetworkError code) {
  qDebug() << code;
}

void AutoUpdateDialog::on_btnStartUpdate_clicked() {
  ui->btnStartUpdate->setEnabled(false);
  startUpdate();
}

void AutoUpdateDialog::startUpdate() {
  ui->btnStartUpdate->setEnabled(false);
  this->repaint();

  QString strZip, strPath, strExec, strUnzip;
  QFileInfo appInfo(qApp->applicationDirPath());
  strZip = tempDir + filename;

  QDir dir;
  dir.setCurrent(tempDir);

  qApp->exit();

  if (mw_one->mac || mw_one->osx1012) {
    QString fileName = tempDir + "up.sh";
    QTextEdit* txtEdit = new QTextEdit();
    QString strTarget = appInfo.path().replace("Contents", "");
    strTarget = strTarget + ".";
    strTarget = "\"" + strTarget + "\"";
    if (mw_one->mac) {
      txtEdit->append("hdiutil mount -mountpoint /Volumes/qtiasl " + strZip);
      txtEdit->append(
          "cp -R -p -f "
          "/Volumes/qtiasl/QtiASL.app/. " +
          strTarget);

      txtEdit->append("hdiutil eject /Volumes/qtiasl");
    }
    if (mw_one->osx1012) {
      txtEdit->append("hdiutil mount -mountpoint /Volumes/qtiasl1012 " +
                      strZip);
      txtEdit->append(
          "cp -R -p -f "
          "/Volumes/qtiasl1012/QtiASL.app/. " +
          strTarget);

      txtEdit->append("hdiutil eject /Volumes/qtiasl1012");
    }

    strPath = appInfo.path().replace("Contents", "");
    strExec = strPath.mid(0, strPath.length() - 1);
    strExec = "\"" + strExec + "\"";
    txtEdit->append("open " + strExec);

    TextEditToFile(txtEdit, fileName);

    QProcess::startDetached("bash", QStringList() << fileName);
  }

  if (mw_one->win) {
    QString fileName = tempDir + "up.bat";
    strPath = appInfo.filePath();

    QTextEdit* txtEdit = new QTextEdit();
    strUnzip = strPath + "/unzip.exe";
    strUnzip = "\"" + strUnzip + "\"";
    strZip = "\"" + strZip + "\"";
    strPath = "\"" + strPath + "\"";
    strExec = qApp->applicationFilePath();
    strExec = "\"" + strExec + "\"";
    QString strCommand1, strCommand2;
    QString strx = "\"" + tempDir + "\"";
    strCommand1 = strUnzip + " -o " + strZip + " -d " + strx;
    QString stry = tempDir + QFileInfo(filename).baseName();
    stry = "\"" + stry + "\"";
    strCommand2 = "xcopy " + stry + " " + strPath + " /s/y";
    txtEdit->append(strCommand1 + " && " + strCommand2 + " && " + strExec);

    TextEditToFile(txtEdit, fileName);

    QProcess::startDetached("cmd.exe", QStringList() << "/c" << fileName);
  }

  if (mw_one->linuxOS) {
    QString fileName = tempDir + "up.sh";
    QTextEdit* txtEdit = new QTextEdit();
    strZip = "\"" + strZip + "\"";
    strLinuxTargetFile = "\"" + strLinuxTargetFile + "\"";
    txtEdit->append("cp -f " + strZip + " " + strLinuxTargetFile);
    txtEdit->append(strLinuxTargetFile);

    TextEditToFile(txtEdit, fileName);

    QProcess::execute("chmod", QStringList() << "+x" << fileName);
    QProcess::startDetached("bash", QStringList() << fileName);
  }
}

void AutoUpdateDialog::startDownload(bool Database) {
  setWindowTitle("");
  ui->label->setVisible(false);
  ui->btnStartUpdate->setEnabled(false);

  this->repaint();

  if (!Database) {
    ui->btnStartUpdate->setVisible(true);

  } else {
    strUrl = strDatabaseUrl;
    ui->btnStartUpdate->setVisible(false);
  }

  QString str0, str1, str2;

  str0 = "https://download.fastgit.org/";            // 日本东京
  str1 = "https://ghproxy.com/https://github.com/";  // 韩国首尔
  str2 = strUrl.replace("https://github.com/", str0);
  strUrl = str2;
  // qDebug() << strUrl;

  QNetworkRequest request;
  request.setUrl(QUrl(strUrl));

  reply = manager->get(request);  //发送请求
  connect(reply, &QNetworkReply::readyRead, this,
          &AutoUpdateDialog::doProcessReadyRead);  //可读
  connect(reply, &QNetworkReply::finished, this,
          &AutoUpdateDialog::doProcessFinished);  //结束
  connect(reply, &QNetworkReply::downloadProgress, this,
          &AutoUpdateDialog::doProcessDownloadProgress);  //大小
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &AutoUpdateDialog::doProcessError);  //异常
  QStringList list = strUrl.split("/");
  filename = list.at(list.length() - 1);
  QDir dir;
  if (dir.mkpath(tempDir)) {
  }
  QString file = tempDir + filename;

  myfile->setFileName(file);
  bool ret =
      myfile->open(QIODevice::WriteOnly | QIODevice::Truncate);  //创建文件
  if (!ret) {
    QMessageBox::warning(this, "warning", "Failed to open.");
    return;
  }
  ui->progressBar->setValue(0);
  ui->progressBar->setMinimum(0);
}

void AutoUpdateDialog::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  reply->close();
}

QString AutoUpdateDialog::GetFileSize(qint64 size) {
  if (!size) {
    return "0 Bytes";
  }
  static QStringList SizeNames;
  if (SizeNames.empty()) {
    SizeNames << " Bytes"
              << " KB"
              << " MB"
              << " GB"
              << " TB"
              << " PB"
              << " EB"
              << " ZB"
              << " YB";
  }
  int i = qFloor(qLn(size) / qLn(1024));
  return QString::number(size * 1.0 / qPow(1000, qFloor(i)), 'f',
                         (i > 1) ? 2 : 0) +
         SizeNames.at(i);
}

void AutoUpdateDialog::TextEditToFile(QTextEdit* txtEdit, QString fileName) {
  QFile* file;
  file = new QFile;
  file->setFileName(fileName);
  bool ok = file->open(QIODevice::WriteOnly);
  if (ok) {
    QTextStream out(file);
    out << txtEdit->toPlainText();
    file->close();
    delete file;
  }
}

void AutoUpdateDialog::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Escape:
      // reply->close();
      // close();
      break;

    case Qt::Key_Return:

      break;

    case Qt::Key_Backspace:

      break;

    case Qt::Key_Space:

      break;

    case Qt::Key_F1:

      break;
  }

  if (event->modifiers() == Qt::ControlModifier) {
    if (event->key() == Qt::Key_M) {
      this->setWindowState(Qt::WindowMaximized);
    }
  }
}
