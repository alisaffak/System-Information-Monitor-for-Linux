#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; 
    w.setWindowIcon(QIcon("/home/ali/SystemMonitor/icon/system.ico") );

    w.show();
    return a.exec();
}
