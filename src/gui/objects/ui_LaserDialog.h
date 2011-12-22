/********************************************************************************
** Form generated from reading UI file 'LaserDialog.ui'
**
** Created
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LASERDIALOG_H
#define UI_LASERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LaserDialog
{
public:
    QVBoxLayout *verticalLayout_5;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *rasterRadio;
    QRadioButton *vectorRadio;
    QRadioButton *combinedRadio;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QSlider *rasterSpeedSlider;
    QLineEdit *rasterSpeed;
    QLabel *label_2;
    QSlider *rasterPowerSlider;
    QLineEdit *rasterPower;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *rasterDirection;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QComboBox *rasterDithering;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *autoFocusBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_8;
    QComboBox *resolutionCombo;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QSlider *vectorSpeedSlider;
    QLineEdit *vectorSpeed;
    QLabel *label_6;
    QSlider *vectorPowerSlider;
    QLineEdit *vectorPower;
    QLabel *label_7;
    QSlider *vectorFreqSlider;
    QLineEdit *vectorFreq;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_9;
    QComboBox *cutOptimize;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *cutButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *LaserDialog)
    {
        if (LaserDialog->objectName().isEmpty())
            LaserDialog->setObjectName(QString::fromUtf8("LaserDialog"));
        LaserDialog->resize(663, 417);
        verticalLayout_5 = new QVBoxLayout(LaserDialog);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tabWidget = new QTabWidget(LaserDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 0, 621, 332));
        gridLayout_3 = new QGridLayout(layoutWidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        groupBox_3 = new QGroupBox(layoutWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        rasterRadio = new QRadioButton(groupBox_3);
        rasterRadio->setObjectName(QString::fromUtf8("rasterRadio"));

        verticalLayout_3->addWidget(rasterRadio);

        vectorRadio = new QRadioButton(groupBox_3);
        vectorRadio->setObjectName(QString::fromUtf8("vectorRadio"));
        vectorRadio->setChecked(true);

        verticalLayout_3->addWidget(vectorRadio);

        combinedRadio = new QRadioButton(groupBox_3);
        combinedRadio->setObjectName(QString::fromUtf8("combinedRadio"));

        verticalLayout_3->addWidget(combinedRadio);


        gridLayout_3->addWidget(groupBox_3, 0, 0, 1, 1);

        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        rasterSpeedSlider = new QSlider(groupBox);
        rasterSpeedSlider->setObjectName(QString::fromUtf8("rasterSpeedSlider"));
        rasterSpeedSlider->setMaximum(100);
        rasterSpeedSlider->setValue(50);
        rasterSpeedSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(rasterSpeedSlider, 0, 1, 1, 1);

        rasterSpeed = new QLineEdit(groupBox);
        rasterSpeed->setObjectName(QString::fromUtf8("rasterSpeed"));

        gridLayout_2->addWidget(rasterSpeed, 0, 2, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        rasterPowerSlider = new QSlider(groupBox);
        rasterPowerSlider->setObjectName(QString::fromUtf8("rasterPowerSlider"));
        rasterPowerSlider->setMaximum(100);
        rasterPowerSlider->setValue(50);
        rasterPowerSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(rasterPowerSlider, 1, 1, 1, 1);

        rasterPower = new QLineEdit(groupBox);
        rasterPower->setObjectName(QString::fromUtf8("rasterPower"));

        gridLayout_2->addWidget(rasterPower, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        rasterDirection = new QComboBox(groupBox);
        rasterDirection->setObjectName(QString::fromUtf8("rasterDirection"));

        horizontalLayout_3->addWidget(rasterDirection);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        rasterDithering = new QComboBox(groupBox);
        rasterDithering->setObjectName(QString::fromUtf8("rasterDithering"));

        horizontalLayout_4->addWidget(rasterDithering);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_4);


        gridLayout_3->addWidget(groupBox, 0, 1, 2, 1);

        groupBox_4 = new QGroupBox(layoutWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_4 = new QVBoxLayout(groupBox_4);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        autoFocusBox = new QCheckBox(groupBox_4);
        autoFocusBox->setObjectName(QString::fromUtf8("autoFocusBox"));
        autoFocusBox->setChecked(true);

        verticalLayout_4->addWidget(autoFocusBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout->addWidget(label_8);

        resolutionCombo = new QComboBox(groupBox_4);
        resolutionCombo->setObjectName(QString::fromUtf8("resolutionCombo"));

        horizontalLayout->addWidget(resolutionCombo);


        verticalLayout_4->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);


        gridLayout_3->addWidget(groupBox_4, 1, 0, 3, 1);

        groupBox_2 = new QGroupBox(layoutWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout = new QFormLayout(groupBox_2);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 0, 0, 1, 1);

        vectorSpeedSlider = new QSlider(groupBox_2);
        vectorSpeedSlider->setObjectName(QString::fromUtf8("vectorSpeedSlider"));
        vectorSpeedSlider->setMaximum(100);
        vectorSpeedSlider->setValue(50);
        vectorSpeedSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(vectorSpeedSlider, 0, 1, 1, 1);

        vectorSpeed = new QLineEdit(groupBox_2);
        vectorSpeed->setObjectName(QString::fromUtf8("vectorSpeed"));

        gridLayout->addWidget(vectorSpeed, 0, 2, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 1, 0, 1, 1);

        vectorPowerSlider = new QSlider(groupBox_2);
        vectorPowerSlider->setObjectName(QString::fromUtf8("vectorPowerSlider"));
        vectorPowerSlider->setMaximum(100);
        vectorPowerSlider->setValue(50);
        vectorPowerSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(vectorPowerSlider, 1, 1, 1, 1);

        vectorPower = new QLineEdit(groupBox_2);
        vectorPower->setObjectName(QString::fromUtf8("vectorPower"));

        gridLayout->addWidget(vectorPower, 1, 2, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 2, 0, 1, 1);

        vectorFreqSlider = new QSlider(groupBox_2);
        vectorFreqSlider->setObjectName(QString::fromUtf8("vectorFreqSlider"));
        vectorFreqSlider->setMaximum(5000);
        vectorFreqSlider->setSingleStep(50);
        vectorFreqSlider->setPageStep(500);
        vectorFreqSlider->setValue(5000);
        vectorFreqSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(vectorFreqSlider, 2, 1, 1, 1);

        vectorFreq = new QLineEdit(groupBox_2);
        vectorFreq->setObjectName(QString::fromUtf8("vectorFreq"));

        gridLayout->addWidget(vectorFreq, 2, 2, 1, 1);


        formLayout->setLayout(0, QFormLayout::SpanningRole, gridLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_5->addWidget(label_9);

        cutOptimize = new QComboBox(groupBox_2);
        cutOptimize->setObjectName(QString::fromUtf8("cutOptimize"));

        horizontalLayout_5->addWidget(cutOptimize);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);


        formLayout->setLayout(1, QFormLayout::SpanningRole, horizontalLayout_5);


        gridLayout_3->addWidget(groupBox_2, 2, 1, 1, 1);

        tabWidget->addTab(tab, QString());

        verticalLayout_5->addWidget(tabWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        cutButton = new QPushButton(LaserDialog);
        cutButton->setObjectName(QString::fromUtf8("cutButton"));

        horizontalLayout_2->addWidget(cutButton);

        cancelButton = new QPushButton(LaserDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setDefault(true);

        horizontalLayout_2->addWidget(cancelButton);


        verticalLayout_5->addLayout(horizontalLayout_2);


        retranslateUi(LaserDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), LaserDialog, SLOT(reject()));
        QObject::connect(cutButton, SIGNAL(clicked()), LaserDialog, SLOT(accept()));

        tabWidget->setCurrentIndex(0);
        resolutionCombo->setCurrentIndex(3);
        cutOptimize->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(LaserDialog);
    } // setupUi

    void retranslateUi(QDialog *LaserDialog)
    {
        LaserDialog->setWindowTitle(QApplication::translate("LaserDialog", "Laser Settings", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("LaserDialog", "Job Type", 0, QApplication::UnicodeUTF8));
        rasterRadio->setText(QApplication::translate("LaserDialog", "Raster", 0, QApplication::UnicodeUTF8));
        vectorRadio->setText(QApplication::translate("LaserDialog", "Vector", 0, QApplication::UnicodeUTF8));
        combinedRadio->setText(QApplication::translate("LaserDialog", "Combined", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("LaserDialog", "Raster Setting", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("LaserDialog", "Speed:", 0, QApplication::UnicodeUTF8));
        rasterSpeed->setText(QApplication::translate("LaserDialog", "50", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("LaserDialog", "Power:", 0, QApplication::UnicodeUTF8));
        rasterPower->setText(QApplication::translate("LaserDialog", "50", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("LaserDialog", "Engrave Direction:", 0, QApplication::UnicodeUTF8));
        rasterDirection->clear();
        rasterDirection->insertItems(0, QStringList()
         << QApplication::translate("LaserDialog", "Top-Down", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Bottom-Up", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("LaserDialog", "Image Dithering:", 0, QApplication::UnicodeUTF8));
        rasterDithering->clear();
        rasterDithering->insertItems(0, QStringList()
         << QApplication::translate("LaserDialog", "Default", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Bayer", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Floyd-Steinberg", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Jarvis", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Burke", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Stucki", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Sierra2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Sierra3", 0, QApplication::UnicodeUTF8)
        );
        groupBox_4->setTitle(QApplication::translate("LaserDialog", "Options", 0, QApplication::UnicodeUTF8));
        autoFocusBox->setText(QApplication::translate("LaserDialog", "Auto Focus", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("LaserDialog", "Resolution:", 0, QApplication::UnicodeUTF8));
        resolutionCombo->clear();
        resolutionCombo->insertItems(0, QStringList()
         << QApplication::translate("LaserDialog", "75 DPI", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "150 DPI", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "300 DPI", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "600 DPI", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "1200 DPI", 0, QApplication::UnicodeUTF8)
        );
        groupBox_2->setTitle(QApplication::translate("LaserDialog", "Vector Setting", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("LaserDialog", "Speed:", 0, QApplication::UnicodeUTF8));
        vectorSpeed->setText(QApplication::translate("LaserDialog", "50", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("LaserDialog", "Power:", 0, QApplication::UnicodeUTF8));
        vectorPower->setText(QApplication::translate("LaserDialog", "50", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("LaserDialog", "Freq:", 0, QApplication::UnicodeUTF8));
        vectorFreq->setText(QApplication::translate("LaserDialog", "5000", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("LaserDialog", "Optimize", 0, QApplication::UnicodeUTF8));
        cutOptimize->clear();
        cutOptimize->insertItems(0, QStringList()
         << QApplication::translate("LaserDialog", "Simple", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Inner-Outer", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LaserDialog", "Shortest Path", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("LaserDialog", "General", 0, QApplication::UnicodeUTF8));
        cutButton->setText(QApplication::translate("LaserDialog", "Save", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("LaserDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LaserDialog: public Ui_LaserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LASERDIALOG_H
