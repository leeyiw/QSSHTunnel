#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("LIYIWEI");
    a.setOrganizationDomain("blog.leeyiw.org");
    a.setApplicationName("QSSHTunnel");

    MainWindow w;
    w.show();

    return a.exec();
}
