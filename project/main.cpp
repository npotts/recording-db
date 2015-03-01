#include "recording_db.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    recording_db w;
    w.show();

    return a.exec();
}
