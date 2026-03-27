#include "widget.h"

#include <QApplication>
#include <QStyleFactory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 应用暗色主题
    a.setStyle(QStyleFactory::create("Fusion"));
    qApp->setStyleSheet("QWidget { background-color: #2b2b2b; }");
    Widget w;
    w.show();
    return QCoreApplication::exec();
}
