/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Qt.hpp"
#include <QMessageBox>

void showErrorDialog(std::string text, std::string detail) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Error!");
  msgBox.setText(QString::fromStdString(text));
  msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.exec();
}

int showWarningDialog(std::string text, std::string detail) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Warning!");
  msgBox.setText(QString::fromStdString(text));
  msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  return msgBox.exec();
}
