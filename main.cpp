#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Set application icon
    QIcon appIcon(":/resources/cat.png");
    a.setWindowIcon(appIcon);
    MainWindow w;
    w.show();
    return a.exec();
}
