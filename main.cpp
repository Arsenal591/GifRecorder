#include <iostream>

#include <QApplication>
#include <QDebug>

#include "mainwindow.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream cout(stdout, QIODevice::WriteOnly);
    cout << msg << '\n';
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
