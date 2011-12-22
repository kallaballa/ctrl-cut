/********************************************************************************
** Form generated from reading UI file 'SimulatorDialog.ui'
**
** Created
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMULATORDIALOG_H
#define UI_SIMULATORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include "SimulatorView.h"

QT_BEGIN_NAMESPACE

class Ui_SimulatorDialog
{
public:
    SimulatorView *graphicsView;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *simulateButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *SimulatorDialog)
    {
        if (SimulatorDialog->objectName().isEmpty())
            SimulatorDialog->setObjectName(QString::fromUtf8("SimulatorDialog"));
        SimulatorDialog->resize(802, 602);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SimulatorDialog->sizePolicy().hasHeightForWidth());
        SimulatorDialog->setSizePolicy(sizePolicy);
        graphicsView = new SimulatorView(SimulatorDialog);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(0, 40, 801, 561));
        widget = new QWidget(SimulatorDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        simulateButton = new QPushButton(widget);
        simulateButton->setObjectName(QString::fromUtf8("simulateButton"));

        horizontalLayout_2->addWidget(simulateButton);

        cancelButton = new QPushButton(widget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setDefault(true);

        horizontalLayout_2->addWidget(cancelButton);


        retranslateUi(SimulatorDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), SimulatorDialog, SLOT(reject()));
        QObject::connect(simulateButton, SIGNAL(clicked()), SimulatorDialog, SLOT(simulate()));

        QMetaObject::connectSlotsByName(SimulatorDialog);
    } // setupUi

    void retranslateUi(QDialog *SimulatorDialog)
    {
        SimulatorDialog->setWindowTitle(QApplication::translate("SimulatorDialog", "Simulator", 0, QApplication::UnicodeUTF8));
        simulateButton->setText(QApplication::translate("SimulatorDialog", "Cut", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("SimulatorDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SimulatorDialog: public Ui_SimulatorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMULATORDIALOG_H
