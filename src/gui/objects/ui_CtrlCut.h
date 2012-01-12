/********************************************************************************
** Form generated from reading UI file 'CtrlCut.ui'
**
** Created
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CTRLCUT_H
#define UI_CTRLCUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "CtrlCutView.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *fileOpenAction;
    QAction *filePrintAction;
    QAction *fileImportAction;
    QAction *toolsMoveToOriginAction;
    QAction *helpAboutAction;
    QAction *simulateAction;
    QAction *deleteItem;
    QAction *simplifyAction;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    CtrlCutView *graphicsView;
    QTableView *settingsTable;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *menuTools;
    QMenu *menuHelp;
    QMenu *editMenu;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(875, 663);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setUnifiedTitleAndToolBarOnMac(true);
        fileOpenAction = new QAction(MainWindow);
        fileOpenAction->setObjectName(QString::fromUtf8("fileOpenAction"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        fileOpenAction->setIcon(icon);
        filePrintAction = new QAction(MainWindow);
        filePrintAction->setObjectName(QString::fromUtf8("filePrintAction"));
        fileImportAction = new QAction(MainWindow);
        fileImportAction->setObjectName(QString::fromUtf8("fileImportAction"));
        toolsMoveToOriginAction = new QAction(MainWindow);
        toolsMoveToOriginAction->setObjectName(QString::fromUtf8("toolsMoveToOriginAction"));
        helpAboutAction = new QAction(MainWindow);
        helpAboutAction->setObjectName(QString::fromUtf8("helpAboutAction"));
        simulateAction = new QAction(MainWindow);
        simulateAction->setObjectName(QString::fromUtf8("simulateAction"));
        deleteItem = new QAction(MainWindow);
        deleteItem->setObjectName(QString::fromUtf8("deleteItem"));
        simplifyAction = new QAction(MainWindow);
        simplifyAction->setObjectName(QString::fromUtf8("simplifyAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        graphicsView = new CtrlCutView(centralwidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        sizePolicy.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(graphicsView);

        settingsTable = new QTableView(centralwidget);
        settingsTable->setObjectName(QString::fromUtf8("settingsTable"));
        sizePolicy.setHeightForWidth(settingsTable->sizePolicy().hasHeightForWidth());
        settingsTable->setSizePolicy(sizePolicy);
        settingsTable->setMinimumSize(QSize(200, 300));
        settingsTable->setMaximumSize(QSize(200, 16777215));
        settingsTable->setAutoScrollMargin(0);
        settingsTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
        settingsTable->setAlternatingRowColors(true);
        settingsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        settingsTable->setSelectionBehavior(QAbstractItemView::SelectItems);
        settingsTable->setSortingEnabled(true);
        settingsTable->setCornerButtonEnabled(false);
        settingsTable->verticalHeader()->setVisible(false);

        horizontalLayout->addWidget(settingsTable);

        horizontalLayout->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 875, 23));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        menuTools = new QMenu(menubar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        editMenu = new QMenu(menubar);
        editMenu->setObjectName(QString::fromUtf8("editMenu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(toolBar->sizePolicy().hasHeightForWidth());
        toolBar->setSizePolicy(sizePolicy2);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(editMenu->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        fileMenu->addAction(fileOpenAction);
        fileMenu->addAction(fileImportAction);
        fileMenu->addSeparator();
        fileMenu->addAction(filePrintAction);
        menuTools->addAction(toolsMoveToOriginAction);
        menuTools->addAction(simulateAction);
        menuHelp->addAction(helpAboutAction);
        editMenu->addAction(deleteItem);
        editMenu->addAction(simplifyAction);
        toolBar->addAction(fileOpenAction);

        retranslateUi(MainWindow);
        QObject::connect(deleteItem, SIGNAL(triggered()), MainWindow, SLOT(on_deleteItem()));
        QObject::connect(simplifyAction, SIGNAL(triggered()), MainWindow, SLOT(on_simplifyItem()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ctrl-Cut", 0, QApplication::UnicodeUTF8));
        fileOpenAction->setText(QApplication::translate("MainWindow", "&Open...", 0, QApplication::UnicodeUTF8));
        fileOpenAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        filePrintAction->setText(QApplication::translate("MainWindow", "&Print...", 0, QApplication::UnicodeUTF8));
        filePrintAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        fileImportAction->setText(QApplication::translate("MainWindow", "&Import", 0, QApplication::UnicodeUTF8));
        fileImportAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+I", 0, QApplication::UnicodeUTF8));
        toolsMoveToOriginAction->setText(QApplication::translate("MainWindow", "&Move to origin", 0, QApplication::UnicodeUTF8));
        helpAboutAction->setText(QApplication::translate("MainWindow", "About...", 0, QApplication::UnicodeUTF8));
        simulateAction->setText(QApplication::translate("MainWindow", "Simulate", 0, QApplication::UnicodeUTF8));
        deleteItem->setText(QApplication::translate("MainWindow", "&Delete", 0, QApplication::UnicodeUTF8));
        deleteItem->setShortcut(QApplication::translate("MainWindow", "Del", 0, QApplication::UnicodeUTF8));
        simplifyAction->setText(QApplication::translate("MainWindow", "Simplify", 0, QApplication::UnicodeUTF8));
        fileMenu->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        editMenu->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CTRLCUT_H
