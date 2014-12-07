#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("LIYIWEI");
    a.setOrganizationDomain("blog.leeyiw.org");
    a.setApplicationName("QSSHTunnel");

    /* i18n */
    QString locale = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_%1.qm").arg(locale),
                      ":/translates/translates");
    qApp->installTranslator(&qtTranslator);
    QTranslator appTranslator;
    appTranslator.load(qApp->applicationName() + "_" + locale + ".qm",
                       ":/translates/translates");
    qApp->installTranslator(&appTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
