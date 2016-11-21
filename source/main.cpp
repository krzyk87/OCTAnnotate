#include "octannotate.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OCTAnnotate w;
    w.show();

    return a.exec();
}
