/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *passwordLineEdit;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QLabel *label_4;
    QPushButton *connectButton;
    QTreeWidget *localFileTree;
    QTreeWidget *remoteFileTree;
    QProgressBar *uploadProgressBar;
    QProgressBar *downloadProgressBar;
    QComboBox *usernameComboBox;
    QComboBox *ipComboBox;
    QCheckBox *savePasswordCheckBox;
    QGraphicsView *graphicsView;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QMenuBar *menubar;
    QMenu *menuHelp;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(871, 575);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        passwordLineEdit = new QLineEdit(centralwidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(150, 185, 150, 30));
        passwordLineEdit->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"background-color: rgb(243, 243, 243);"));
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(35, 115, 106, 22));
        label->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(60, 155, 91, 22));
        label_2->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(40, 195, 81, 20));
        label_3->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(150, 225, 151, 30));
        lineEdit->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"background-color: rgb(243, 243, 243);"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(105, 225, 41, 31));
        label_4->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(175, 270, 100, 35));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 12, 177, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush2(QColor(255, 255, 255, 128));
        brush2.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush2);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush2);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush2);
#endif
        connectButton->setPalette(palette);
        QFont font;
        font.setPointSize(12);
        font.setBold(false);
        connectButton->setFont(font);
        connectButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 12, 177); color: white; border-radius: 5px"));
        localFileTree = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        localFileTree->setHeaderItem(__qtreewidgetitem);
        localFileTree->setObjectName("localFileTree");
        localFileTree->setGeometry(QRect(319, 75, 250, 385));
        localFileTree->setAcceptDrops(true);
        localFileTree->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"selection-background-color: rgb(0, 12, 177);\n"
"background-color: rgb(243, 243, 243);"));
        localFileTree->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
        localFileTree->setDragEnabled(true);
        localFileTree->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
        localFileTree->setDefaultDropAction(Qt::DropAction::CopyAction);
        localFileTree->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        localFileTree->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
        localFileTree->setWordWrap(true);
        remoteFileTree = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QString::fromUtf8("1"));
        remoteFileTree->setHeaderItem(__qtreewidgetitem1);
        remoteFileTree->setObjectName("remoteFileTree");
        remoteFileTree->setGeometry(QRect(580, 75, 250, 385));
        remoteFileTree->setAcceptDrops(true);
        remoteFileTree->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"background-color: rgb(243, 243, 243);\n"
"selection-background-color: rgb(0, 12, 177);\n"
""));
        remoteFileTree->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
        remoteFileTree->setDragEnabled(true);
        remoteFileTree->setDragDropOverwriteMode(false);
        remoteFileTree->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
        remoteFileTree->setDefaultDropAction(Qt::DropAction::CopyAction);
        remoteFileTree->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        remoteFileTree->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
        remoteFileTree->setSortingEnabled(true);
        remoteFileTree->setWordWrap(true);
        uploadProgressBar = new QProgressBar(centralwidget);
        uploadProgressBar->setObjectName("uploadProgressBar");
        uploadProgressBar->setGeometry(QRect(315, 470, 250, 24));
        uploadProgressBar->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"selection-background-color: rgb(0, 12, 177);\n"
"color: rgb(255, 255, 255);"));
        uploadProgressBar->setValue(24);
        downloadProgressBar = new QProgressBar(centralwidget);
        downloadProgressBar->setObjectName("downloadProgressBar");
        downloadProgressBar->setGeometry(QRect(580, 470, 251, 24));
        downloadProgressBar->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"selection-background-color: rgb(0, 12, 177);\n"
"color: rgb(255, 255, 255);"));
        downloadProgressBar->setValue(24);
        usernameComboBox = new QComboBox(centralwidget);
        usernameComboBox->setObjectName("usernameComboBox");
        usernameComboBox->setGeometry(QRect(150, 145, 150, 30));
        usernameComboBox->setAutoFillBackground(false);
        usernameComboBox->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"background-color: rgb(243, 243, 243);"));
        usernameComboBox->setEditable(true);
        ipComboBox = new QComboBox(centralwidget);
        ipComboBox->setObjectName("ipComboBox");
        ipComboBox->setGeometry(QRect(150, 105, 150, 30));
        ipComboBox->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"font: 600 12pt \"Noto Sans\";\n"
"background-color: rgb(243, 243, 243);"));
        ipComboBox->setEditable(true);
        savePasswordCheckBox = new QCheckBox(centralwidget);
        savePasswordCheckBox->setObjectName("savePasswordCheckBox");
        savePasswordCheckBox->setGeometry(QRect(124, 195, 16, 16));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(savePasswordCheckBox->sizePolicy().hasHeightForWidth());
        savePasswordCheckBox->setSizePolicy(sizePolicy);
        savePasswordCheckBox->setSizeIncrement(QSize(20, 20));
        savePasswordCheckBox->setStyleSheet(QString::fromUtf8("color: rgb(17, 0, 255);\n"
"background-color: rgb(255, 255, 255);\n"
"border-color: rgb(255, 255, 255);\n"
""));
        graphicsView = new QGraphicsView(centralwidget);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setGeometry(QRect(60, 320, 191, 176));
        graphicsView->setStyleSheet(QString::fromUtf8("border-image: url(:/cat.png);"));
        graphicsView->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(355, 0, 166, 36));
        QFont font1;
        font1.setPointSize(32);
        font1.setBold(true);
        font1.setItalic(true);
        label_5->setFont(font1);
        label_5->setStyleSheet(QString::fromUtf8("color: rgb(0, 12, 177);"));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(319, 75, 251, 31));
        QFont font2;
        font2.setPointSize(14);
        font2.setBold(true);
        label_6->setFont(font2);
        label_6->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(0, 12, 177);"));
        label_6->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(580, 75, 250, 31));
        label_7->setFont(font2);
        label_7->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(0, 12, 177);"));
        label_7->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(300, 40, 276, 26));
        label_8->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(375, 515, 120, 25));
        label_9->setStyleSheet(QString::fromUtf8("font: 12pt \"Noto Sans\";\n"
""));
        label_9->setTextFormat(Qt::TextFormat::RichText);
        label_9->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_9->setOpenExternalLinks(true);
        MainWindow->setCentralWidget(centralwidget);
        passwordLineEdit->raise();
        label->raise();
        label_2->raise();
        label_3->raise();
        lineEdit->raise();
        label_4->raise();
        localFileTree->raise();
        remoteFileTree->raise();
        uploadProgressBar->raise();
        downloadProgressBar->raise();
        usernameComboBox->raise();
        ipComboBox->raise();
        savePasswordCheckBox->raise();
        graphicsView->raise();
        label_5->raise();
        label_7->raise();
        label_8->raise();
        label_9->raise();
        connectButton->raise();
        label_6->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 871, 27));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuHelp->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SSHFiles", nullptr));
        passwordLineEdit->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "Host Address:", nullptr));
#if QT_CONFIG(whatsthis)
        label_2->setWhatsThis(QCoreApplication::translate("MainWindow", "<html><head/><body><p>color: rgb(230, 230, 230);</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        label_2->setText(QCoreApplication::translate("MainWindow", "Username:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Password:", nullptr));
        lineEdit->setText(QCoreApplication::translate("MainWindow", "22", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
#if QT_CONFIG(accessibility)
        connectButton->setAccessibleName(QString());
#endif // QT_CONFIG(accessibility)
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
#if QT_CONFIG(tooltip)
        savePasswordCheckBox->setToolTip(QCoreApplication::translate("MainWindow", "Save Password?", nullptr));
#endif // QT_CONFIG(tooltip)
        savePasswordCheckBox->setText(QCoreApplication::translate("MainWindow", "Save Password?", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "SSHFiles", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Local Files", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Remote Files", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Share files with a remote SSH server.", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><a href=\"https://beeralator.com\"><span style=\" text-decoration: underline; color:#0f1bff;\">Beeralator.com</span></a></p></body></html>", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
