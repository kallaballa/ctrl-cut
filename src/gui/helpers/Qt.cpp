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
  if(!detail.empty())
    msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.exec();
}

void showWarningDialog(std::string text, std::string detail) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Warning!");
  msgBox.setText(QString::fromStdString(text));
  if(!detail.empty())
    msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.exec();
}

int showYesNoDialog(std::string title, std::string text, std::string detail) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(QString::fromStdString(title));
  msgBox.setText(QString::fromStdString(text));
  if(!detail.empty())
    msgBox.setDetailedText(QString::fromStdString(detail));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  return msgBox.exec();
}
