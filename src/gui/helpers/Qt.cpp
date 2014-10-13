#include "Qt.hpp"
#include <QMessageBox>

void showErrorDialog(std::string text, std::string detail) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Error!");
  msgBox.setText(QString::fromStdString(text));
  msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.exec();
}
