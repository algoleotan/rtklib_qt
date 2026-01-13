/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *txtFile1;
    QPushButton *btnLoadFile1;
    QPushButton *btnClearFile1;
    QCheckBox *chkShowFile1;
    QLabel *label_2;
    QLineEdit *txtFile2;
    QPushButton *btnLoadFile2;
    QPushButton *btnClearFile2;
    QCheckBox *chkShowFile2;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnRefresh;
    QSpacerItem *horizontalSpacer;
    QLabel *lblDisplay;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 800);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        txtFile1 = new QLineEdit(groupBox);
        txtFile1->setObjectName(QString::fromUtf8("txtFile1"));
        txtFile1->setReadOnly(true);

        gridLayout->addWidget(txtFile1, 0, 1, 1, 1);

        btnLoadFile1 = new QPushButton(groupBox);
        btnLoadFile1->setObjectName(QString::fromUtf8("btnLoadFile1"));

        gridLayout->addWidget(btnLoadFile1, 0, 2, 1, 1);

        btnClearFile1 = new QPushButton(groupBox);
        btnClearFile1->setObjectName(QString::fromUtf8("btnClearFile1"));

        gridLayout->addWidget(btnClearFile1, 0, 3, 1, 1);

        chkShowFile1 = new QCheckBox(groupBox);
        chkShowFile1->setObjectName(QString::fromUtf8("chkShowFile1"));
        chkShowFile1->setChecked(true);

        gridLayout->addWidget(chkShowFile1, 0, 4, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        txtFile2 = new QLineEdit(groupBox);
        txtFile2->setObjectName(QString::fromUtf8("txtFile2"));
        txtFile2->setReadOnly(true);

        gridLayout->addWidget(txtFile2, 1, 1, 1, 1);

        btnLoadFile2 = new QPushButton(groupBox);
        btnLoadFile2->setObjectName(QString::fromUtf8("btnLoadFile2"));

        gridLayout->addWidget(btnLoadFile2, 1, 2, 1, 1);

        btnClearFile2 = new QPushButton(groupBox);
        btnClearFile2->setObjectName(QString::fromUtf8("btnClearFile2"));

        gridLayout->addWidget(btnClearFile2, 1, 3, 1, 1);

        chkShowFile2 = new QCheckBox(groupBox);
        chkShowFile2->setObjectName(QString::fromUtf8("chkShowFile2"));
        chkShowFile2->setChecked(true);

        gridLayout->addWidget(chkShowFile2, 1, 4, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnRefresh = new QPushButton(groupBox);
        btnRefresh->setObjectName(QString::fromUtf8("btnRefresh"));

        horizontalLayout->addWidget(btnRefresh);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 5);


        verticalLayout->addWidget(groupBox);

        lblDisplay = new QLabel(centralwidget);
        lblDisplay->setObjectName(QString::fromUtf8("lblDisplay"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblDisplay->sizePolicy().hasHeightForWidth());
        lblDisplay->setSizePolicy(sizePolicy);
        lblDisplay->setFrameShape(QFrame::Box);
        lblDisplay->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lblDisplay);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "AlgoMemsZeroBiasPlot - MEMS Zero Bias Viewer", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "File Selection", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "File 1:", nullptr));
        btnLoadFile1->setText(QCoreApplication::translate("MainWindow", "Load File 1...", nullptr));
        btnClearFile1->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        chkShowFile1->setText(QCoreApplication::translate("MainWindow", "Show", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "File 2:", nullptr));
        btnLoadFile2->setText(QCoreApplication::translate("MainWindow", "Load File 2...", nullptr));
        btnClearFile2->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        chkShowFile2->setText(QCoreApplication::translate("MainWindow", "Show", nullptr));
        btnRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        lblDisplay->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
