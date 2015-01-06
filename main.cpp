#include "widget.h"
#include <QApplication>

#include <cstddef>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    std::srand(123);

    return a.exec();
}
