#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("🚗 赛车游戏 - Qt Game Collection");
    w.show();
    return a.exec();
}